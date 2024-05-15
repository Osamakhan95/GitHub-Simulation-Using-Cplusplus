#ifndef HEADER_H
#define HEADER_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<Windows.h>

using namespace std;

class User;

struct Repository 
{
	string name;
	bool isPublic;
	vector<string> commits;
	vector<string> files;
	vector<string> fileContents;
	vector<Repository*> children;
	vector<Repository*> forks;
	Repository(const string& repoName, bool publicFlag) : name(repoName), isPublic(publicFlag) {}
	void Add_Child(Repository* child) 
	{
		children.push_back(child);
	}
};

// Define User structure
class User 
{
public:
	string passwordHash;
	Repository* rootRepository;
	string key;
	vector<string> followers;
	vector<string> followedUsers;
	User() : rootRepository(nullptr) {}
	User(const string& passHash) : passwordHash(passHash), rootRepository(nullptr) {}
	void Add_Edge(User* user) 
	{
		followedUsers.push_back(user->key);
		user->followers.push_back(this->key);
	}
	void Remove_Edge(User* user) 
	{
		for (size_t i = 0; i < followedUsers.size(); ++i) 
		{
			if (followedUsers[i] == user->key) 
			
			{
				followedUsers.erase(followedUsers.begin() + i);
				break;
			}
		}
		for (size_t i = 0; i < user->followers.size(); ++i) 
		{
			if (user->followers[i] == this->key) 
			{
				user->followers.erase(user->followers.begin() + i);
				break;
			}
		}
	}
};
struct HashTable 
{
	static const int TABLE_SIZE = 1000;
	vector<pair<string, User*>> table[TABLE_SIZE];
	int Hash_Function(const string& key) 
	{
		int hash = 0;
		for (size_t i = 0; i < key.size(); ++i) 
		{
			hash = (hash * 31 + key[i]) % TABLE_SIZE;
		}
		return hash;
	}
	void Insert(const string& key, User* value) 
	{
		int index = Hash_Function(key);
		table[index].push_back(make_pair(key, value));
	}
	User* Find(const string& key) 
	{
		int index = Hash_Function(key);
		for (size_t i = 0; i < table[index].size(); ++i)
		{
			if (table[index][i].first == key) 
			{
				return table[index][i].second;
			}
		}
		return nullptr;
	}
};

extern HashTable User_Database;
class Graph 
{
private:
	vector<pair<string, vector<string>>> adjList;
public:
	void Add_Vertex(const string& vertex) 
	{
		auto it = find_if(adjList.begin(), adjList.end(), [&](const auto& pair) 
			{
			return pair.first == vertex;
			});
		if (it == adjList.end()) 
		{
			adjList.push_back({ vertex, {} });
		}
	}
	void Add_Edge(const string& from, const string& to) 
	{
		Add_Vertex(from);
		Add_Vertex(to);
		auto it = find_if(adjList.begin(), adjList.end(), [&](const auto& pair) {
			return pair.first == from;
			});
		if (it != adjList.end()) 
		{
			it->second.push_back(to);
		}
	}
	void Remove_Edge(const string& from, const string& to) 
	{
		User* follower = User_Database.Find(from);
		if (follower != nullptr) 
		{
			auto it = find(follower->followedUsers.begin(), follower->followedUsers.end(), to);
			if (it != follower->followedUsers.end()) 
			{
				follower->followedUsers.erase(it);
				cout << "-Edge removed: '" << from << "' is no longer following '" << to << "'." << endl;
			}
			else 
			{
				cout << "Error: User '" << from << "' cannot following '" << to << "'." << endl;
			}
		}
		else 
		{
			cout << "-Error: Follower username cannot found." << endl;
		}
	}
	const vector<string>& Get_Adjacent_Vertices(const string& vertex) 
	{
		static const vector<string> emptyVector;
		auto it = find_if(adjList.begin(), adjList.end(), [&](const auto& pair) 
			{
			return pair.first == vertex;
			});
		if (it != adjList.end()) 
		{
			return it->second;
		}

		return emptyVector;
	}
};
void Save_Users_In_File(const string& filename);
void Loading_Users_From_File(const string& filename);
void Save_Repository_In_File(const Repository* repo);
bool Register_User(const string& username, const string& password);
bool Login_User(const string& username, const string& password);
void View_Profile(const string& username);
void Create_Repository(const string& username, const string& repoName, bool isPublic);
void Add_File_In_Repository(const string& username, const string& repoName, const string& fileName, const string& fileContents);
void Add_Commit(const string& username, const string& repoName, const string& commit);
void Delete_File_From_Repository(const string& username, const string& repoName, const string& fileName);
void Delete_Repository(const string& username, const string& repoName);
void Fork_Repository(const string& username, const string& sourceUsername, const string& sourceRepoName);
void View_Repository_Stats(const string& username, const string& repoName);
void Follow_User(const string& followerUsername, const string& followedUsername);
void Unfollow_User(const string& followerUsername, const string& followedUsername);
void Process_Choice();
void Loading_Page();

#endif