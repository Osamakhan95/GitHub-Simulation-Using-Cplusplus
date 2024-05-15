#include"Header.h"

HashTable User_Database;

void Save_Users_In_File(const string& File_Name) 
{


	ofstream file(File_Name);
	if (file.is_open()) 
	{

		for (int i = 0; i < HashTable::TABLE_SIZE; ++i)
		{

			for (const auto& entry : User_Database.table[i])
			{

				file << entry.first << " " << entry.second->passwordHash;
				file << " | ";
				file << "| ";
				if (entry.second->rootRepository != nullptr) 
				{

					file << entry.second->rootRepository->name << " ";
				}

				file << endl;
			}
		}
		cout << "-The User Database saved into the file." << endl;
	}

	else 
	{

		cout << "-Error: Cannot open user data file for writing." << endl;
	}
	file.close();
}

void Loading_Users_From_File(const string& File_Name) 
{

	ifstream file(File_Name);
	if (file.is_open()) 
	{

		string line;
		while (getline(file, line)) 
		{

			stringstream ss(line);
			string User_Name, Password_Hash, Data;
			ss >> User_Name >> Password_Hash;
			User* newUser = new User();
			newUser->passwordHash = Password_Hash;
			string temp;
			while (ss >> temp && temp != "|") 
			{
				newUser->followers.push_back(temp);
			}
			while (ss >> temp && temp != "|") 
			{
				newUser->followedUsers.push_back(temp);
			}
			while (ss >> temp) 
			{
				newUser->rootRepository = new Repository(temp, true);
			}
			User_Database.Insert(User_Name, newUser);
		}
		
	}
	else 
	{
		return;
	}
	file.close();
}
void Save_Repository_In_File(const Repository* repositry) 
{
	ofstream outFile(repositry->name + ".csv");
	if (outFile.is_open()) 
	{
		outFile << "Repositry Name: " << repositry->name << endl;
		outFile << "Visibility: " << (repositry->isPublic ? "Public" : "Private") << endl;
		outFile << "Commits:" << endl;
		for (const string& commit : repositry->commits) 
		{
			outFile << "- " << commit << endl;
		}
		outFile << "Files:" << endl;
		for (size_t i = 0; i < repositry->files.size(); ++i) 
		{
			outFile << "- " << repositry->files[i] << endl;
			outFile << "  " << repositry->fileContents[i] << endl;
		}
		outFile.close();
		cout << "-Repository data saved to file." << endl;
	}
	else 
	{
		cout << "-Error: Cannot open repository data file for writing." << endl;
	}
}
bool Register_User(const string& username, const string& password) 
{
	if (User_Database.Find(username) != nullptr) 
	{
		cout << "Error: Username already exists." << endl;
		return false;
	}
	string passwordHash = password;
	User* newUser = new User();
	newUser->passwordHash = passwordHash;
	newUser->rootRepository = nullptr;
	User_Database.Insert(username, newUser);
	cout << "-Registration successful." << endl;
	Save_Users_In_File("users.csv");
	return true;
}
bool Login_User(const string& username, const string& password) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr) 
	{
		if (user->passwordHash != password) 
		{
			cout << "-Error: Wrong Password." << endl;
			return false;
		}
		cout << "-Login Successful." << endl;
		return true;
	}
	cout << "-Error: Wrong Username." << endl;
	return false;
}

void Create_Repository(const string& username, const string& repoName, bool isPublic) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr) 
	{
		if (user->rootRepository == nullptr) 
		{
			user->rootRepository = new Repository(repoName, isPublic);
			cout << "-Repository created successfully." << endl;
			Save_Repository_In_File(user->rootRepository);
		}
		else 
		{
			cout << "-Error: User already has a repository." << endl;
		}
	}
	else 
	{
		cout << "-Error: Username cannot found." << endl;
	}
}
void Add_File_In_Repository(const string& username, const string& repoName, const string& fileName, const string& fileContents) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr && user->rootRepository != nullptr) 
	{
		Repository* repo = user->rootRepository;
		repo->files.push_back(fileName);
		repo->fileContents.push_back(fileContents);
		cout << "-File added to repository '" << repoName << "'." << endl;
		Save_Repository_In_File(repo);
	}
	else 
	{
		cout << "-Error: Username or repository cannot found." << endl;
	}
}
void Add_Commit(const string& username, const string& repoName, const string& commit) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr && user->rootRepository != nullptr) 
	{
		Repository* repo = user->rootRepository;
		repo->commits.push_back(commit);
		cout << "-Commit added to repository '" << repoName << "'." << endl;
		Save_Repository_In_File(repo);
	}
	else 
	{
		cout << "-Error: Username or repository cannot found." << endl;
	}
}
void Delete_File_From_Repository(const string& username, const string& repoName, const string& fileName) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr && user->rootRepository != nullptr) 
	{
		Repository* repo = user->rootRepository;
		auto fileIt = find(repo->files.begin(), repo->files.end(), fileName);
		if (fileIt != repo->files.end()) 
		{
			size_t index = distance(repo->files.begin(), fileIt);
			repo->files.erase(fileIt);
			repo->fileContents.erase(repo->fileContents.begin() + index);
			cout << "File '" << fileName << "' deleted from repository '" << repoName << "'." << endl;
			Save_Repository_In_File(repo);
		}
		else 
		{
			cout << "-Error: File '" << fileName << "' cannot found in repository '" << repoName << "'." << endl;
		}
	}
	else 
	{
		cout << "-Error: Username or repository cannot found." << endl;
	}
}
void Delete_Repository(const string& username, const string& repoName) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr) 
	{
		if (user->rootRepository != nullptr && user->rootRepository->name == repoName) 
		{
			delete user->rootRepository;
			user->rootRepository = nullptr;
			cout << "-Repository '" << repoName << "' deleted successfully." << endl;
		}
		else 
		{
			cout << "-Error: Repository cannot found or you don't have access to delete it." << endl;
		}
	}
	else 
	{
		cout << "-Error: Username cannot found." << endl;
	}
}

void Fork_Repository(const string& username, const string& sourceUsername, const string& sourceRepoName) 
{
	User* user = User_Database.Find(username);
	User* sourceUser = User_Database.Find(sourceUsername);
	if (user != nullptr && sourceUser != nullptr) 
	{
		if (sourceUser->rootRepository != nullptr) 
		{
			if (!sourceUser->rootRepository->isPublic)
			{
				cout << "-Error: Cannot fork private repositories." << endl;
				return;
			}
			Create_Repository(username, sourceRepoName, sourceUser->rootRepository->isPublic);
			user = User_Database.Find(username);
			Repository* newRepo = user->rootRepository;
			Repository* sourceRepo = sourceUser->rootRepository;
			newRepo->commits = sourceRepo->commits;
			newRepo->files = sourceRepo->files;
			sourceRepo->forks.push_back(newRepo);

			cout << "-Repository forked successfully." << endl;
			Save_Repository_In_File(newRepo);
		}
		else 
		{
			cout << "-Error: Source repository cannot found." << endl;
		}
	}
	else 
	{
		cout << "Error: Wrongs username(s)." << endl;
	}
}
void View_Repository_Stats(const string& username, const string& repoName) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr) 
	{
		Repository* repo = user->rootRepository;
		if (repo != nullptr && repo->name == repoName) 
		{
			ifstream inFile(repoName + ".csv");
			if (inFile.is_open()) 
			{
				string line;
				while (getline(inFile, line)) 
				{
					cout << line << endl;
				}
				inFile.close();
			}
			else 
			{
				cout << "-Error: Cannot open repository data file for reading." << endl;
			}
		}
		else 
		{
			cout << "-Error: Repository cannot found." << endl;
		}
	}
	else 
	{
		cout << "-Error: Username cannot found." << endl;
	}
}

void Update_Followers_In_File(const string& followerUsername, const string& followedUsername) 
{
	ofstream file("followers.txt", ios::app);
	if (file.is_open()) 
	{
		file << followerUsername << " " << followedUsername << endl;
		file.close();
	}
	else 
	{
		cout << "-Error: Unable to open file for writing." << endl;
	}
}
void Follow_User(const string& followerUsername, const string& followedUsername) 
{
	User* follower = User_Database.Find(followerUsername);
	User* followedUser = User_Database.Find(followedUsername);
	if (follower != nullptr && followedUser != nullptr) 
	{
		follower->followedUsers.push_back(followedUsername);
		Update_Followers_In_File(followerUsername, followedUsername);
		cout << "-User '" << followerUsername << "' is now following '" << followedUsername << "'." << endl;
	}
	else
	{
		cout << "-Error: Wrong usernames." << endl;
	}

}

void View_Profile(const string& username) 
{
	User* user = User_Database.Find(username);
	if (user != nullptr) 
	{
		ifstream followersFile("followers.txt");
		string line;
		cout << "-Username: " << username << endl;
		cout << "-Followers: ";
		bool found = false;
		while (getline(followersFile, line)) 
		{
			istringstream iss(line);
			string followedUser, follower;
			if (iss >> followedUser >> follower) 
			{
				if (followedUser == username) 
				{
					cout << follower << " ";
					found = true;
				}
			}
		}
		followersFile.close();
		if (!found) 
		{
			cout << "-No followers found.";
		}
		cout << endl;
		cout << "-Following: ";
		for (const auto& followedUser : user->followedUsers) 
		{
			cout << followedUser << " ";
		}
		cout << endl;
		cout << "-Repositories: ";
		if (user->rootRepository) 
		{
			cout << user->rootRepository->name << " ";
		}
		cout << endl;
	}
	else 
	{
		cout << "-Error: Username cannot found." << endl;
	}
}
void Load_Followers_From_File(const string& filename) 
{
	ifstream file(filename);
	if (file.is_open()) 
	{
		string line;
		while (getline(file, line)) 
		{
			stringstream ss(line);
			string followedUser, follower;
			if (ss >> followedUser >> follower) 
			{
				User* user = User_Database.Find(followedUser);
				if (user != nullptr) 
				{
					user->followers.push_back(follower);
				}
				else 
				{
					cout << "-Error: Followed user '" << followedUser << "' cannot found." << endl;
				}
			}
			else 
			{
				cout << "-Error: Wrong data in followers file." << endl;
			}
		}
		cout << "-Followers data loaded from file." << endl;
	}
	else 
	{
		cout << "-Error: Cannot open followers file for reading." << endl;
	}
	file.close();
}
void Unfollow_User(const string& followerUsername, const string& followedUsername) 
{
	Graph g;
	User* follower = User_Database.Find(followerUsername);
	if (follower != nullptr) 
	{
		auto it = find(follower->followedUsers.begin(), follower->followedUsers.end(), followedUsername);
		if (it != follower->followedUsers.end()) 
		{
			follower->followedUsers.erase(it);
			cout << "-User '" << followerUsername << "' has unfollowed '" << followedUsername << "'." << endl;
			g.Remove_Edge(followerUsername, followedUsername);
		}
		else 
		{
			cout << "Error: User '" << followerUsername << "' is not following '" << followedUsername << "'." << endl;
		}
	}
	else {
		cout << "Error: Follower username not found." << endl;
	}
}



void Clear_Screen() 
{
	cout << "Press Enter to continue...";
	cin.ignore();
	cin.get();
	system("cls");
}
void Process_Choice() 
{
	string username, password, repoName, commit, fileName, fileContents, sourceUsername, sourceRepoName;
	bool isPublic;
	string choice;
	bool loggedIn = false;
	Loading_Users_From_File("users.csv");
	while (true) 
	{
		if (!loggedIn) 
		{
			cout << "\n1. Register\n2. Login\n3. Exit\n";
		}
		else 
		{
			Clear_Screen();
			cout << "\n4. View_Profile\n5. Create_Repository\n6. Add_Commit\n";
			cout << "7. Delete_Repository\n8. Fork_Repository\n9. View_Repository_Stats\n";
			cout << "10. Add_File_to_Repository\n11. Delete_File_from_Repository\n";
			cout << "12. Follow_User\n13. Unfollow_User\n14. Logout\n15. Exit\n";
		}
		cout << "-Enter your choice: ";
		cin >> choice;
		if (!loggedIn) 
		{
			if (choice == "1") 
			{
				cout << "-Enter username: ";
				cin.ignore();
				getline(cin, username);
				cout << "Enter password: ";
				getline(cin, password);
				Register_User(username, password);
				Clear_Screen();
			}
			else if (choice == "2") {
				cout << "-Enter username: ";
				cin.ignore();
				getline(cin, username);
				cout << "-Enter password: ";
				getline(cin, password);
				if (Login_User(username, password)) 
				{
					loggedIn = true;
				}
				Clear_Screen();
			}
			else if (choice == "3") 
			{
				Save_Users_In_File("users.csv");
				cout << "-Exiting program. Bye-Bye" << endl;
				Clear_Screen();
				exit(0);
			}
			else 
			{
				cout << "Wrong choice. Please try again." << endl;
			}
		}
		else 
		{
			if (choice == "4") 
			{
				View_Profile(username);
				Clear_Screen();
			}
			else if (choice == "5") 
			{
				cout << "-Enter repository name: ";
				cin.ignore();
				getline(cin, repoName);
				cout << "-Is the repository public? (1 for yes, 0 for no): ";
				cin >> isPublic;
				Create_Repository(username, repoName, isPublic);
				Clear_Screen();
			}
			else if (choice == "6") 
			{
				cout << "-Enter repository name: ";
				cin.ignore();
				getline(cin, repoName);
				cout << "-Enter commit message: ";
				getline(cin, commit);
				Add_Commit(username, repoName, commit);
				Clear_Screen();
			}
			else if (choice == "7") 
			{
				cin.ignore();
				cout << "-Enter repository name: ";
				getline(cin, repoName);
				Delete_Repository(username, repoName);
				Clear_Screen();
			}
			else if (choice == "8") 
			{
				cin.ignore();
				cout << "-Enter source username: ";
				getline(cin, sourceUsername);
				cout << "-Enter source repository name: ";
				getline(cin, sourceRepoName);
				Fork_Repository(username, sourceUsername, sourceRepoName);
				Clear_Screen();
			}
			else if (choice == "9")
			{
				cin.ignore();
				cout << "-Enter repository name: ";
				getline(cin, repoName);
				Clear_Screen();
				View_Repository_Stats(username, repoName);
			}
			else if (choice == "10") 
			{
				cin.ignore();
				cout << "-Enter repository name: ";
				getline(cin, repoName);
				cout << "-Enter file name: ";
				getline(cin, fileName);
				cout << "-Enter file contents: ";
				cin.ignore();
				getline(cin, fileContents);
				Add_File_In_Repository(username, repoName, fileName, fileContents);
				Clear_Screen();
			}
			else if (choice == "11")
			{
				cin.ignore();
				cout << "-Enter repository name: ";
				getline(cin, repoName);
				cout << "-Enter file name: ";
				getline(cin, fileName);
				Delete_File_From_Repository(username, repoName, fileName);
				Clear_Screen();
			}
			else if (choice == "12") 
			{
				cin.ignore();
				cout << "-Enter followed username: ";
				getline(cin, sourceUsername);
				Follow_User(username, sourceUsername);
				Save_Users_In_File("users.csv");
				Clear_Screen();
			}
			else if (choice == "13") 
			{
				cin.ignore();
				cout << "-Enter followed username: ";
				getline(cin, sourceUsername);
				Unfollow_User(username, sourceUsername);
				Clear_Screen();
			}
			else if (choice == "14") 
			{
				loggedIn = false;
				cout << "-Logged out successfully." << endl;
			}
			else if (choice == "15") 
			{
				Save_Users_In_File("users.csv");
				cout << "-Exiting program. Goodbye!" << endl;
				Clear_Screen();
				exit(0);
			}
			else 
			{
				cout << "-Wrong choice. Please try again." << endl;
			}
		}
	}
}

void Draw_Progress_Bar(int width, int progress) 
{
	int barWidth = width - 2; // Account for borders
	int filledWidth = (barWidth * progress) / 100;

	cout << "\t\t\t\t[";
	for (int i = 0; i < barWidth; i++)
	{
		if (i < filledWidth)
		{
			cout << "#";
		}
		else
		{
			cout << " ";
		}
	}
	cout << "] " << progress << "%";
}

void Draw_Board(int width, int height) 
{
	cout << "\n\n";
	for (int i = 0; i < height; i++) 
	{
		cout << "\t\t\t\t#";
		for (int j = 0; j < width - 2; j++) 
		{
			if (i == height / 2) {
				if (j == (width - 2) / 2 - 5) 
				{
					cout << "-GITHUB Simulation-";
					j += 20;
				}
				else 
				{
					cout << "#";
				}
			}
			else 
			{
				cout << " ";
			}
		}
		cout << "#\n";
	}
	cout << "\n\n";
}

void Loading_Page() 
{
	int boardWidth = 60;
	int boardHeight = 10;
	Draw_Board(boardWidth, boardHeight);

	string names[] = { "Usama Jamil", "Rana Arham", "Owais Qarni" };
	string loadingText = "\n\n\t\t\t\t\tLoading";
	const int loadingLength = 20;

	for (const string& name : names) 
	{
		cout << "\n\n\t\t\t\t";
		for (char letter : name) 
		{
			cout << letter;
			Sleep(150); 
		}
	}

	cout << loadingText;

	for (int i = 0; i < loadingLength; i++) 
	{
		Sleep(100);
		cout << ".";
	}

	cout << "\n\n\t\t\t\t";
	for (int percent = 0; percent <= 100; percent += 10) 
	{
		Sleep(200); 
		cout << "\r"; 
		Draw_Progress_Bar(40, percent);
	}
	system("CLS"); 
}

