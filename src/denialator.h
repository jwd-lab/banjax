/*
*  A subfilter of banjax that deny access to any
*  bot which has been reported to swabber. it reply a message
*
*  Vmon: Dec 2015: Initial version.
*/
#ifndef DENIALATOR_H
#define DENIALATOR_H

#include "swabber.h"
#include "banjax_filter.h"

class GlobalWhiteList;

class Denialator final : public BanjaxFilter
{
 protected:
  //swabber object used for banning bots after grace period is finished
  Swabber* swabber;
  GlobalWhiteList* global_white_list;

  long banning_grace_period = 0;

  Swabber::IpDb* swabber_ip_db;
 public:
  /**
     receives the config object need to read the ip list,
     subsequently it reads all the ips

  */
 Denialator(const FilterConfig& filter_config,
            Swabber::IpDb* swabber_ip_db,
            Swabber* swabber,
            GlobalWhiteList* global_white_list)
   : BanjaxFilter(DENIALATOR_FILTER_ID,
                  DENIALATOR_FILTER_NAME),
     swabber(swabber),
     global_white_list(global_white_list),
     swabber_ip_db(swabber_ip_db)
  {
    queued_tasks[HTTP_REQUEST] = this;
    banning_grace_period = swabber->get_grace_period();
    load_config(merge_config(filter_config));
  }

  /**
    Overload of the load config
    reads all the regular expressions from the database.
    and compile them
  */
  void load_config(const YAML::Node&) {};

  /**
     Overloaded to tell banjax that we need url, host, ua and ip
     for banning
     At this point we only asks for the ip
     later we can ask more if it is needed
   */
  uint64_t requested_info() { return TransactionMuncher::IP;}    

  /**
     overloaded execute to execute the filter, it assemble the
     parts to make ats record and then call the parse log
   */
  FilterResponse on_http_request(const TransactionParts& transaction_parts) override;
  void on_http_close(const TransactionParts& transaction_parts) override {}

  /**
     we  overload generate_respons cause we have to say denied access
  */
  std::string generate_response(const TransactionParts& transaction_parts, const FilterResponse& response_info) override;

};
  
#endif /* white_lister.h */
