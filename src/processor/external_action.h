#ifndef EXTERNAL_ACTION_H
#define EXTERNAL_ACTION_H
#include <string>
using namespace std;

/* action generated by LogEntryProcessor */
struct externalAction
{
	string action; // action in configuration
	string argument1; // first argument (for example ip or host)
	string argument2; // second argument (modelname, previous action)
	string argument3;
	externalAction(string &action,string &argument1,string &argument2,string &argument3):
		action(action),
		argument1(argument1),
		argument2(argument2),
		argument3(argument3)
	{

	}
	externalAction(string &action,string &argument1,string &argument2):
			action(action),
			argument1(argument1),
			argument2(argument2),
			argument3()
		{

		}
};

#endif
