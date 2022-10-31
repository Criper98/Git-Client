#pragma once

void StampaTitolo(string Version, short NewLine = 0)
{
	TextColor tc;
	tc.SetColor(tc.Green);
	
	cout << "   ________.__  __    _________ .__  .__               __   " << endl;
	cout << "  /  _____/|__|/  |_  \\_   ___ \\|  | |__| ____   _____/  |_ " << endl;
	cout << " /   \\  ___|  \\   __\\ /    \\  \\/|  | |  |/ __ \\ /    \\   __\\" << endl;
	cout << " \\    \\_\\  \\  ||  |   \\     \\___|  |_|  \\  ___/|   |  \\  |  " << endl;
	cout << "  \\______  /__||__|    \\______  /____/__|\\___  >___|  /__|  " << endl;
	cout << "         \\/                   \\/             \\/     \\/      " << endl;
	cout << "| Versione: " << Version << endl;
	cout << "| By Criper98" << endl;
	
	tc.SetColor(tc.Default);
	
	for(short i = 0; i < NewLine; i++)
		cout << endl;
}

VectString LoadSavedRepo( GitProjects& Projects, Log& log, bool Debug)
{
	DirUtils du;
	VectString Paths;
	VectString RepoNames;
	ifstream infile;
	string FileName = "projects.ini";
	string buff;
	
	Projects.clear();
	du.ChangeCurrDir(du.GetFilePath());
	
	if ( !du.CheckFile( FileName ) )
	{
		du.WriteFile( FileName );
		return RepoNames;
	}
	
	infile.open( FileName.c_str() );
	while ( getline(infile, buff ) )
		Paths.push_back( buff );
	infile.close();
	
	if( Paths.size() == 0 )
		return RepoNames;
	
	for (int i = 0; i < Paths.size(); i++)
	{
		Projects.push_back( GitProject() );
		Projects[i].RepoPath = Paths[i];
	}
	
	if (Debug)
		log.WriteLog("Recupero nomi dei repo salvati.");
	
	for (int i = 0; i < Projects.size(); i++)
		RepoNames.push_back( Projects[i].GetRepoName(log, Debug) );
	
	if (Debug)
		log.WriteLog("----------------");
	
	return RepoNames;
}

bool SaveRepoPath( string Path, bool Debug, Log& log )
{
	DirUtils du;
	GitProject Git;
	string FileName = "projects.ini";
	
	Git.RepoPath = Path;
	
	if(Debug)
		log.WriteLog("Salvo il percorso del repo: " + Git.RepoPath);
	
	if( Git.IsRepo() )
	{
		du.ChangeCurrDir( du.GetFilePath() );
		
		return du.AppendToFile( FileName, Path );
	}
	
	return false;
}

void RemoveRepo( GitProjects& Projects, int Selection)
{
	DirUtils du;
	string FileName = "projects.ini";
	
	du.ChangeCurrDir( du.GetFilePath() );
	
	Projects.erase( Projects.begin() + Selection );
	
	du.WriteFile( FileName );
	
	for (int i = 0; i < Projects.size(); i++)
		du.AppendToFile( FileName, Projects[i].RepoPath );
}

void CheckSettings(SettingsFile& sf)
{
	if(!sf.CheckSetting("Debug"))
		sf.SetSetting("Debug", "false");
	
	if(!sf.CheckSetting("ConsoleSizeX"))
		sf.SetSetting("ConsoleSizeX", "700");
	
	if(!sf.CheckSetting("ConsoleSizeY"))
		sf.SetSetting("ConsoleSizeY", "500");
}

VectSettings LoadSettings(SettingsFile& sf)
{
	DirUtils du;
	
	VectSettings Settings;
	
	du.ChangeCurrDir(du.GetFilePath());
	
	CheckSettings(sf);
	
	Settings.push_back(SettingsMenu());
	Settings[0].Name = "Salva e Torna Indietro";
	Settings[0].Escape = true;
	
	Settings.push_back(SettingsMenu());
	Settings[1].Name = "Debug";
	Settings[1].CheckValue = "true";
	Settings[1].Value = sf.GetSetting("Debug");
	
	Settings.push_back(SettingsMenu());
	Settings[2].Name = "Dimensione Console X,Y";
	Settings[2].Escape = true;
	Settings[2].Value = sf.GetSetting("ConsoleSizeX");
	Settings[2].SecValue = sf.GetSetting("ConsoleSizeY");
	
	return Settings;
}

short CreateRepo(Log& log, bool Debug, string Path)
{
	GeneralUtils gu;
	DirUtils du;
	
	int Rtn = -1;
	string Out;
	
	du.ChangeCurrDir(Path);
	
	Out = gu.GetCMDOutput("git init", Rtn);
	
	if(Debug)
		log.WriteLog("Git init: " + to_string(Rtn) + "\n" + Out);
	
	if(Rtn == 0)
	{
		gu.NoOutputCMD("git add .");
		gu.NoOutputCMD("git commit -m \"Initial commit\"");
		return 0;
	}
	else
	{
		log.WriteLog("Errore durante la creazione del repo: " + to_string(Rtn) + "\n" + Out);
		return -1;
	}
	
	return -1;
}

void AssociateRemoteRepo(Log& log, bool Debug, GitProjects& Projects, int ActiveRepo)
{
	TextColor tc;
	CLInterface cli;
	GeneralUtils gu;
	DirUtils du;
	
	string URL;
	
	tc.SetColor(tc.Yellow);
	cout << "Nessun Repository remoto indicato.\nAssociarne uno ora?" << endl;
	
	if(cli.MenuSingleSelQuadre({ "Si", "No" }) == 0)
	{
		cout << "URL Repository: ";
		cin >> URL;
		du.ChangeCurrDir(Projects[ActiveRepo].RepoPath);
		gu.NoOutputCMD("git remote add origin " + URL);
		tc.SetColor(tc.Lime);
		cout << "Repository associato." << endl;
		
		if(Debug)
			log.WriteLog("Repo [" + Projects[ActiveRepo].GetRepoName(log, Debug) + "] Associato con URL [" + URL + "]");
	}
	else
	{
		tc.SetColor(tc.Purple);
		cout << "Associazione annullata." << endl;
	}
	
	tc.SetColor(tc.Default);
}

bool UpdateRepoList(GitProjects& Projects, int ActiveRepo)
{
	string Buff = Projects[ActiveRepo].RepoPath;
	DirUtils du;

	for (int i = 0; i < Projects.size(); i++)
		if (Projects[i].RepoPath != Projects[ActiveRepo].RepoPath)
			Buff += "\n" + Projects[i].RepoPath;

	du.ChangeCurrDir(du.GetFilePath());
	return du.WriteFile("projects.ini", Buff + "\n");
}