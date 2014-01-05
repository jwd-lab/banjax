#include "../log_entry.h"
#include "../features/feature.h"
#include "../features/feature_container.h"
#include "bot_banger_aggregator.h"
#include <algorithm>


/* register a feature at index, features are owned
 */
void BotBangerAggregator::RegisterFeature(Feature *f,int index)
{
	_features.push_back(pair<Feature *,int>(f,index));
	_maxFeatureNum=max(index+1,_maxFeatureNum);
	_featureMemoryNeeded+=f->GetDataSize();
}

/* constructor, max mappings and session length can be set
 */
BotBangerAggregator::BotBangerAggregator(int maxEntries,int sessionLength):
	LogAggregator(),
	_maxFeatureNum(-1),
	_featureMemoryNeeded(0),
	_maxEntries(maxEntries),
	_sessionLength(sessionLength)
{

}

/* clean up all entries and listeners
 */
void BotBangerAggregator::Cleanup()
{
	// delete eventlisteners
	for (auto i=_eventListeners.begin();i!=_eventListeners.end();i++) delete (*i);
	// delete features
	for (auto i=_features.begin();i!=_features.end();i++) delete (*i).first;
	// delete feature values
	for (auto i=_map.begin();i!=_map.end();i++) delete (*i).second;	
}

/* destructor
 */
BotBangerAggregator::~BotBangerAggregator()
{
	Cleanup();
}

/* helper function for qsort in prune
 */
bool SortAges(pair<string,time_t> a,pair<string,time_t> b)
{
	return a.second<b.second; // we want a reverse sort
}

/* flush the oldest 10%
 */
void BotBangerAggregator::Prune() {
	// prune 10%, we should never hit this, but if we do
	// it will not be a problem that it is not the fastest way
	// if this becomes a problem we need a proper LRU structure
	// there is one included in lru_cache_using_std
	// we would need to switch to scoped ptrs for that
	vector<pair<string, time_t> > ages;
	ages.reserve(_map.size());
	for (auto i = _map.begin(); i != _map.end(); i++) {
		ages.push_back(
				pair<string, time_t>((*i).first, (*i).second->lastRequestTime));
	}
	std::sort(ages.begin(), ages.end(), SortAges); // qsort , thus quick
	int prune = _map.size() / 10;
	for (auto i = ages.begin(); i != ages.end() && prune != 0; i++, prune--) {
		auto f = _map.find((*i).first);
		delete (*f).second;
		_map.erase(f);
		for(auto i=_eventListeners.begin();i!=_eventListeners.end();i++)
		{
			(*i)->OnEvictEvent((*f).first);
		}
	}
}

/* return predicted memory usage for current configuration
 */
int BotBangerAggregator::PredictedMemoryUsage()
{
	// this is all ballpark stuff, not exact
	return
			(
					20+ // IP address length (guestimate)
					32+ // map overhead per entry (guestimate)
					GetFeatureSupportMemory()+
					sizeof(double)*(GetMaxFeatureIndex()+1)+
					sizeof(FeatureContainer)
			)
			*
			_maxEntries;
}

/* aggregrate logentry and calculate registered features
 */
void BotBangerAggregator::Aggregate(LogEntry * le)
{
	
	string useraddress=string(le->useraddress);
	auto f=_map.find(useraddress);
	FeatureContainer *c=NULL;
	

	if (f!=_map.end()) // entry found
	{
		c=(*f).second;
		if ((le->endTime-_sessionLength)>c->lastRequestTime) // check session timeout
			c->Clear();
	}
	else // entry not found, create new entry
	{
		if (_map.size()>=_maxEntries) // entry will overflow map
		{
			Prune(); // prune 10%
		}
		_map[useraddress]=c=new FeatureContainer(this); // add new entry
	}

	c->Aggregrate(le); // calculate features
	
	// pass onto listeners
	for(auto i=_eventListeners.begin();i!=_eventListeners.end();i++)
	{
		(*i)->OnFeatureEvent(le->useraddress,c->GetFeatureData(),this->GetMaxFeatureIndex());
	}

}
