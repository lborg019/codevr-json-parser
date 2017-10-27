#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <algorithm>
#include <stack>
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

namespace ns
{
	// Outtermost nodes:
	//Expr
	struct expression {
		string ast_type;
		json value;
	};

	//FunctionDef
	struct functiondef {
		json args;
		string ast_type;
		json body;
		string name;
	};

	//Assign
	struct assign {
		string ast_type;
		vector<json> targets;
		json value;
	};

	struct _assign {
		string ast_type;
		string targets;
		string value;
	};

	// Inner nodes:
	//Call
	struct call {
		json args;
		string ast_type;
		json func;
		//keywords
		//lineno
	};

	//Name
	struct name {
		string ast_type;
		json ctx;
		string id;
	};

	//func
	struct func {
		string ast_type;
		string id;
	};

	struct num {
		string ast_type;
		json n;
	};

	//assign inner nodes:
	struct assign_value {};
	struct assign_targets {};

	// (might be useless)
	struct module {
		expression e;
		functiondef f;
		assign a;
	};
}


int main()
{
	cout << "rapid json test ";
	ifstream infile("./file2.json");
	if (!infile)
	{
		cout << "File not found!\n";
		return 1;
	}

	json j;
	infile >> j;

	// range-based for
	for (auto& element : j) {
		if (element.is_array()) {
			for (size_t i = 0; i < element.size(); i++)
			{
				cout << "\n***ELEMENT: " << i << endl;
				for (json::iterator it = element.at(i).begin(); it != element.at(i).end(); ++it)
				{
					if (it.key() == "ast_type")
					{
						// Parse Assign node
						if (it.value() == "Assign")
						{
							ns::_assign _a;

							ns::assign a{
								element.at(i)["ast_type"].get<string>(),
								element.at(i)["targets"].get<vector<json>>(),
								element.at(i)["value"].get<json>()
							};

							_a.ast_type = a.ast_type; // save type

							//parse target (left hand side of assignment)
							//targets is an array; traverse array:
							for (size_t i = 0; i < a.targets.size(); i++)
							{
								json t = a.targets.at(i);
								//iterate over json objects
								for (json::iterator it = t.begin(); it != t.end(); it++)
								{
									if (it.key() == "ast_type" && it.value() == "Name")
									{
										ns::name n{
											a.targets.at(i)["ast_type"].get<string>(),
											a.targets.at(i)["ctx"].get<json>(),
											a.targets.at(i)["id"].get<string>()
										};
										_a.targets = n.id; // save target
									}
								}
							}

							//parse value (right hand side of assignment)
							json v = a.value; //value is not an array; iterate right away
							for (json::iterator it = v.begin(); it != v.end(); it ++)
							{
								if (it.key() == "ast_type" && it.value() == "Call")
								{
									//parse call
									ns::call c{
										v["args"].get<json>(),
										v["ast_type"].get<string>(),	
										v["func"].get<json>()
									};
									
									//parse inner call's func
									json f = c.func;
									for (json::iterator it = f.begin(); it != f.end(); it++)
									{
										if (it.key() == "ast_type" && it.value() == "Name")
										{
											ns::func fun{
												f["ast_type"].get<string>(),
												f["id"].get<string>()
											};

											_a.value = fun.id + c.args.dump(); // save value
										}
									}
								}
								else if (it.key() == "ast_type" && it.value() == "Num")
								{
									// do recursive inner calls to Assign
									// in order to test this part of the code

									//parse num
									ns::num n{
										v["ast_type"].get<string>(),
										v["n"].get<json>()
									};
									cout << "CAUGHT A NUM" << endl;
								}
							}

							cout << _a.ast_type << ":" << endl;
							cout << _a.targets << " = " << _a.value << endl;
							// push loaded _a to a vector or whatever datastructure
						}
						else if (it.value() == "Expr")
						{
							ns::expression e
							{
								element.at(i)["ast_type"].get<string>(),
								element.at(i)["value"].get<json>()
							};

							cout << "ast_type: " << e.ast_type << endl;
							cout << "value: " << e.value << endl;

						}
						else if (it.value() == "FunctionDef")
						{
							ns::functiondef f
							{
								element.at(i)["args"].get<json>(),
								element.at(i)["ast_type"].get<string>(),
								element.at(i)["body"].get<json>(),
								element.at(i)["name"].get<string>()
							};

							cout << "args: " << f.args << endl;
							cout << "ast_type: " << f.ast_type << endl;
							cout << "body: " << f.body << endl;
							cout << "name: " << f.name << endl;
						}
					}
				}

			}
		}

	}
	system("PAUSE");
	return 0;
}