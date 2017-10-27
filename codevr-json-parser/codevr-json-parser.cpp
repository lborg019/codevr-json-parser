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
		//json targets;
		vector<json> targets;
		json value;
	};

	// Inner nodes:
	//Call
	struct call {
		string ast_type;
		int col_offset;
		//func[]
		//keywords[]
		int lineno;
	};

	//Name
	struct name {
		string ast_type;
		json ctx;
		string id;
	};

	struct value {};

	struct targets {};

	// (might be useless)
	struct module {
		expression e;
		functiondef f;
		assign a;
	};

	//void from json
}


int main()
{
	cout << "rapid json test ";
	ifstream infile("./file1.json");
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
			//cout << setw(4) << element << endl;
			for (size_t i = 0; i < element.size(); i++)
			{
				cout << "\n***ELEMENT: " << i << endl;
				//cout << element.at(i) << endl;
				for (json::iterator it = element.at(i).begin(); it != element.at(i).end(); ++it)
				{
					if (it.key() == "ast_type")
					{
						//cout << it.value() << endl;
						// Parse Assign node
						if (it.value() == "Assign")
						{
							//cout << "found assign" << endl;
							ns::assign a{
								element.at(i)["ast_type"].get<string>(),
								element.at(i)["targets"].get<vector<json>>(),
								//element.at(i)["targets"].get<json>()
								element.at(i)["value"].get<json>()
							};

							cout << "ast_type: " << a.ast_type << endl;
							cout << "targets: " << a.targets << endl;
							cout << "value: " << a.value << endl;

							for (size_t i = 0; i < a.targets.size(); i++)
							{
								json t = a.targets.at(i);
								for (json::iterator it = t.begin(); it != t.end(); it++)
								{
									if (it.key() == "ast_type")
									{
										if (it.value() == "Name")
										{
											ns::name n{
												a.targets.at(i)["ast_type"].get<string>(),
												a.targets.at(i)["ctx"].get<json>(),
												a.targets.at(i)["id"].get<string>()
											};

											cout << "ast_type: " << n.ast_type << endl;
											cout << "ctx: " << n.ctx << endl;
											cout << "id: " << n.id << endl;
										}
									}
								}

							}
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