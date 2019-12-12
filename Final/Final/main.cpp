//Cameron Westlake
//Final

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <conio.h>

using namespace std;

class ConnectionData
{
public:
	char node;
	float length;
	ConnectionData(char node, float length)
	{
		this->node = node;
		this->length = length;
	}
};

class NodeData
{
public:
	int x;
	int y;
	char name;
	vector<ConnectionData*> conn;
	NodeData(int x, int y, char name)
	{
		this->x = x;
		this->y = y;
		this->name = name;
	}
};

class MappingData
{
public:
	vector<MappingData*> ls;
	NodeData* node;
	MappingData(NodeData* node) 
	{
		this->node = node;
	}
};

class CostData
{
public:
	string path;
	int cost;
	CostData(string path, int cost)
	{
		this->path = path;
		this->cost = cost;
	}
};


NodeData* findNode(vector<NodeData*> &list, char node)
{
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i]->name == node)
			return list[i];
	}
	return NULL;
}

vector<MappingData*> findBreadthResult(vector<MappingData*> indata, NodeData* endnode, vector<NodeData*>& glist, bool first)
{
	vector<MappingData*> output;
	for (int i = 0; i < indata.size(); i++)
	{
		if (indata[i]->node == endnode && !first)
		{
			output.push_back(indata[i]);
			return output;
		}
		else
		{
			vector<MappingData*> input = findBreadthResult(indata[i]->ls, endnode, glist, false);
			if (input.size() != 0)
			{
				input.insert(input.begin(), indata[i]);
				return input;
			}
		}
	}
	return output;
}

vector<MappingData*> breadthFirstMap(NodeData* startnode, vector<NodeData*>& glist)
{
	//This might leak memory really badly but oh well
	vector<MappingData*> output;
	vector<MappingData*> currlist;
	vector<char> visited;
	currlist.push_back(new MappingData(startnode));
	output = currlist;
	
	cout << "-------BREADTH FIRST-------\n";

	int layer = 0;
	while(currlist.size() > 0)
	{
		
		layer++;
		bool layerprint = true;

		vector<MappingData*> newlist;
		for (int i = 0; i < currlist.size(); i++)
		{
			int poss = 0;
			for (int j = 0; j < currlist[i]->node->conn.size(); j++)
			{
				bool nottaken = true;
				for (int k = 0; k < visited.size(); k++)
				{
					if (currlist[i]->node->conn[j]->node == visited[k])
						nottaken = false;
				}
				if (nottaken)
				{
					poss++;
					MappingData* newdat = new MappingData(findNode(glist, currlist[i]->node->conn[j]->node));
					currlist[i]->ls.push_back(newdat);
					newlist.push_back(newdat);
					visited.push_back(newdat->node->name);
					if (layerprint)
					{
						cout << "\nLayer: " << layer << endl;
						layerprint = false;
					}
					cout << currlist[i]->node->name << "-" << newdat->node->name << endl;
				}
			}
		}
		currlist = newlist;
	}
	cout << endl;
	return output;
}

CostData* findLowestCost(NodeData* startnode, NodeData* endnode, vector<NodeData*> &glist, vector<char> visited, NodeData* currentnode, string &currentpath)
{

	if (currentnode == endnode && visited.size() > 0)
	{
		string arggg = "";
		return new CostData(arggg, 0);
	}
	int output = 0;
	if(!(startnode == endnode && endnode == currentnode))
		visited.push_back(currentnode->name);
	bool anyvisited = false;
	int lowestinlist = 999999;
	NodeData* lowestnode = NULL;
	CostData* lowest = NULL;
	string currpaths = currentpath;
	for (int i = 0; i < currentnode->conn.size(); i++)
	{
		
		
		bool notvisited = true;
		for (int j = 0; j < visited.size(); j++)
		{
			if (currentnode->conn[i]->node == visited[j])
				notvisited = false;
		}
		if (notvisited)
		{
			CostData* thiscost = findLowestCost(startnode, endnode, glist, visited, findNode(glist, currentnode->conn[i]->node), currentpath);
			if (thiscost != NULL)
			{
				int tempval = currentnode->conn[i]->length + thiscost->cost;
				if (lowestinlist > tempval)
				{
					anyvisited = true;
					lowestinlist = tempval;
					lowestnode = findNode(glist, currentnode->conn[i]->node);
					lowest = thiscost;
				}
			}
		}
	}
	if (anyvisited)
	{
		if (lowest != NULL)
		{
			lowest->cost = lowestinlist;
			lowest->path = "-" + (lowestnode->name + lowest->path);
		}
	}
	else
	{
		if (currentnode != endnode)
		{
			//string bleh = "";
			//lowest = new CostData(bleh, 999999);
		}
	}
	return lowest;
}

int main()
{
	
	if (!filesystem::exists("paths"))
	{
		if (!filesystem::create_directory("paths"))
		{
			cout << "Failed to create path!\n";
			system("pause");
			return 0;
		}
	}
	
	//Parsing files into something easier to deal with
	vector<string> filenames;
	for (filesystem::directory_entry i : filesystem::directory_iterator("paths"))
	{
		filenames.push_back(i.path().filename().generic_string());
	}

	if (filenames.size() == 0)
	{
		cout << "No Valid files!\n";
		system("pause");
		return 0;
	}

	bool selected = false;
	int pointer = 0;
	while (!selected)
	{
		system("CLS");
		cout << "Please select a file\ns/w/Enter\n";
		for (int i = 0; i < filenames.size(); i++)
		{
			if (i == pointer)
				cout << ">";
			else
				cout << " ";
			cout << filenames[i] << endl;
		}
		cout << endl;
		int input = _getch();
	
		if (input == 'w')//Up
		{
			if (pointer > 0)
				pointer--;
		}
		else if (input == 's')//Down
		{
			if (pointer < filenames.size() - 1)
				pointer++;
		}
		else if (input == 13)//Enter
		{
			selected = true;
		}
	}

	//Converting file data into a more useful form, because nothing in c++ works apparently and i have to do it like this
	vector<string> filelines;
	{
		ifstream file;
		file.open("paths/" + filenames[pointer]);
		filebuf* buf = file.rdbuf();
		int nexc = 0;
		string currline = "";
		while ((nexc = buf->sbumpc()) != -1)
		{
			if (nexc == '\n')
			{
				filelines.push_back(currline);
				currline = "";
			}
			else
				currline += nexc;
		}
		filelines.push_back(currline);
	}

	//Time to parse
	vector<NodeData*> nodes;
	char endnode = ' ';
	for (int i = 0; i < filelines.size(); i++)
	{
		string cl = filelines[i];
		if (cl[0] == '-')//Marks start pos
		{
			endnode = cl[1];
		}
		else if (cl[0] == '^')//Marks connection
		{
			char node1 = cl[1];
			char node2 = cl[3];
			float num = atof(&cl[5]);
			NodeData* snode = findNode(nodes, node1);
			NodeData* enode = findNode(nodes, node2);
			snode->conn.push_back(new ConnectionData(enode->name, num));
			enode->conn.push_back(new ConnectionData(snode->name, num));
		}
		else//Likely just adding a node
		{
			//int num1 = 0;
			//int num2 = 0;
			nodes.push_back(new NodeData(0, 0, cl[0]));
		}
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		cout << nodes[i]->name << " connections: ";
		for (int j = 0; j < nodes[i]->conn.size(); j++)
		{
			cout << nodes[i]->conn[j]->node << "-" << nodes[i]->conn[j]->length << " ";
		}
		cout << endl;
		
	}
	cout << "Start node: " << nodes[0]->name << endl;
	cout << "End node: " << endnode << endl;

	//Parsing data done! Now to actually search
	//Breadth first?
	vector<MappingData*> breadthfirst = breadthFirstMap(nodes[0], nodes);
	vector<MappingData*> breadthpath = findBreadthResult(breadthfirst, findNode(nodes, endnode), nodes, true);

	//And print
	cout << "Breadth path: ";
	for (int i = 0; i < breadthpath.size(); i++)
	{
		cout << breadthpath[i]->node->name;
		if (i + 1 < breadthpath.size())
			cout << "-";

	}
	cout << endl;
	vector<char> visited;
	string path = "";
	CostData* lowestcost = findLowestCost(nodes[0], findNode(nodes, endnode), nodes, visited, nodes[0], path);
	cout << "\nLowest Cost: " << lowestcost->cost << "\nPath: " << nodes[0]->name + lowestcost->path << endl << endl << endl;


	system("pause");
	return 0;
}