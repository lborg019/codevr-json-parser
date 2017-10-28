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
	// clean it eventually
	struct call {
		json args;
		string ast_type;
		json func;
	};

	struct _call {
		string id;
		vector<string> args;
	};

	// Outtermost nodes:
	//Expr (useless)
	/*struct expression {
		string ast_type;
		json value;
	};*/

	struct _expression
	{
		string ast_type;
		_call call;
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

	//FunctionDef
	struct functiondef {
		json args;
		string ast_type;
		json body;
		string name;
	};

	// Inner nodes:

	//Name
	struct name {
		string ast_type;
		json ctx;
		string id;
	};

	struct _name {
		string name;
	};

	//func
	struct func {
		string ast_type;
		string id;
	};

	struct number {
		string ast_type; //int, double, float
		string val; //3, 3.1, 3.14159
	};

	//assign inner nodes:
	struct assign_value {};
	struct assign_targets {};
}

// helper method to parse name:
//ns::name parseName(){}

// helper method to output AssignNode:
void outputAssign(ns::_assign a)
{
	//string ast_type;
	string targets = a.targets;
	string value = a.value;
	cout << a.targets << " = " << a.value << endl;
}

// helper method to output calls:
string formatCall(ns::_call _c){
	string ca = _c.id;
	//cout << _c.id;
	if (_c.args.size() != 0)
	{
		//cout << "(";
		ca += "(";
		for (size_t i = 0; i < _c.args.size(); i++)
		{
			if (i != _c.args.size() - 1)
				//cout << _c.args.at(i) << ",";
				ca += _c.args.at(i) + ",";
			else
				//cout << _c.args.at(i);
				ca += _c.args.at(i);

		}
		//cout << ")" << endl;
		ca += ")";
	}
	else
		//cout << "()" << endl;
		ca += "()";

	return ca;
}

// helper method to parse a Call node:
ns::_call parseCall(json::iterator it, json v)
{
	ns::_call _c;
	ns::call c{
		v["args"].get<json>(),
		v["ast_type"].get<string>(),
		v["func"].get<json>()
	};

	// arguments must be a vector of strings:
	// print("hello")                       string
	// print(sayHello())                    call
	// print("myRelation(%d, %d)" % (p,q))  BinOp[str + op(right(tuple, elts(load(p), load(q))))]

	//[Call] parse args;
	json a_array = c.args;
	vector<string> argStr = vector<string>();
	if (a_array.size() != 0)
	{
		for (size_t i = 0; i < a_array.size(); i++)
		{
			json a = a_array.at(i);
			for (json::iterator it = a.begin(); it != a.end(); it++)
			{
				if (it.key() == "ast_type" && it.value() == "Name")
				{
					// usually 'Loads'
					argStr.push_back(a["id"].get<string>());
				}
				else if (it.key() == "ast_type" && it.value() == "Num")
				{
					json num = a["n"].get<json>();
					ns::number n{
						num["ast_type"].get<string>(),
						to_string(num["n"].get<int>())
					};
					//cout << n.ast_type << " : " << n.val << endl;
					argStr.push_back(n.val);
				}
				else if (it.key() == "ast_type" && it.value() == "Str")
				{
					argStr.push_back(a["s"].get<string>());
				}
				else if (it.key() == "ast_type" && it.value() == "Tuple")
					cout << "found a tuple" << endl; //parse Tuple [implement]
				else if (it.key() == "ast_type" && it.value() == "BinOp")
					cout << "found a BinOp" << endl; //parse BinOp [implement]
			}
		}
	}
	_c.args = argStr; //save list of arguments

	//[Call] parse func for id and attribute;
	json f = c.func;
	for (json::iterator it = f.begin(); it != f.end(); it++)
	{
		if (it.key() == "ast_type" && it.value() == "Attribute")
		{
			/*
			"func": {
				"ast_type": "Attribute",
				"attr": "add",
				(...)
				"value": {
					"ast_type": "Name"
					(...)
					"id": "orderedPairsSet"
				}
			}
			*/
			//parse value
			//concatenate attribute with id
		}
		else if (it.key() == "ast_type" && it.value() == "Name")
		{
			_c.id = f["id"].get<string>();
		}
	}

	/* Abstract call into:
		callID + callAttribute
		callArguments
	*/

	return _c;
}

// helper method to parse expressions:
ns::_expression parseExpr(json element, size_t i)
{
	ns::_expression _e;
	_e.ast_type = element.at(i)["ast_type"].get<string>();

	json v = element.at(i)["value"].get<json>();
	for (json::iterator it = v.begin(); it != v.end(); it++)
	{
		if (it.key() == "ast_type" && it.value() == "Call")
		{
			ns::_call _c = parseCall(it, v);
			_e.call = _c;
		}
	}
	return _e;
}

ns::_assign parseAssign(json element, size_t i)
{
	ns::_assign _a;
	ns::assign a{
		element.at(i)["ast_type"].get<string>(),
		element.at(i)["targets"].get<vector<json>>(),
		element.at(i)["value"].get<json>()
	};

	_a.ast_type = a.ast_type;
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
				//save target
				_a.targets = a.targets.at(i)["id"].get<string>();
			}
		}
	}
	//parse value (right hand side of assignment)
	json v = a.value; //value is not an array; iterate right away
	for (json::iterator it = v.begin(); it != v.end(); it++)
	{
		if (it.key() == "ast_type" && it.value() == "Call")
		{
			ns::_call _c;
			_c = parseCall(it, v);
			//cout << formatCall(_c) << endl;
			_a.value = formatCall(_c);
		}
		else if (it.key() == "ast_type" && it.value() == "Num")
		{
			//parse num
			json num = v["n"].get<json>();
			ns::number n{
				num["ast_type"].get<string>(),
				to_string(num["n"].get<int>())
			};
			// save value
			_a.value = n.val;
		}
		//else if(it.key() == "ast_type" && it.value() == "BinOp"){}
	}

	return _a;
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
			for (size_t i = 0; i < element.size(); i++)
			{
				cout << "\n***ELEMENT: " << i << endl;
				for (json::iterator it = element.at(i).begin(); it != element.at(i).end(); ++it)
				{
					if (it.key() == "ast_type")
					{
						if (it.value() == "Assign") // done!
						{
							ns::_assign _a = parseAssign(element, i);
							outputAssign(_a);
						}
						else if (it.value() == "Expr") // done!
						{
							ns::_expression _e = parseExpr(element, i);
							cout << formatCall(_e.call) << endl;
						}
						else if (it.value() == "FunctionDef")
						{
							//encompasses both Expr and Assignment
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