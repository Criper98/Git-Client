#pragma once

class GitProject
{
	private:
		GeneralUtils gu;
		DirUtils du;
		//atomic<bool> Stop = false; // da errore, probabilmente non è compatibile coi vettori :(
		bool Stop = false;
		
		void SC(Log& log, bool Debug, COORD CursorPos)
		{
			TextColor tc;
			WindowUtils wu;
			ConsoleUtils cu;
			
			bool Update = true;
			COORD OrigPos;
			
			Sleep(2000);
			while(!Stop)
			{
				if(wu.IsWindowActive() && Update)
				{
					Update = false;
					OrigPos = cu.GetCursorPos();
					cu.SetCursorPos(CursorPos);
					cout << "                                   ";
					cu.SetCursorPos(CursorPos);
					cout << GetStatus(log, Debug); tc.SetColor(tc.Default); cout << " ]";
					cu.SetCursorPos(OrigPos);
				}
				else if(!wu.IsWindowActive())
				{
					Update = true;
				}
				Sleep(500);
			}
		}
	
	public:
		string RepoPath;
		
		string GetRepoName(Log& log, bool Debug)
		{
			du.ChangeCurrDir(RepoPath);
			string RepoName = RepoPath;
			
			while (RepoName.find("\\") != string::npos)
				RepoName.erase(0, RepoName.find("\\") + 1);
			
			while (RepoName.find(" ") != string::npos)
				RepoName.replace(RepoName.find(" "), 1, "-");
			
			if (Debug)
				log.WriteLog("Nome repo: " + RepoName);
			
			return RepoName;
		}
		
		string GetActiveBranch(Log& log, bool Debug)
		{
			du.ChangeCurrDir(RepoPath);
			string Branch = SimpleFind(gu.GetCMDOutput("git branch"), "* ", "\n");
			
			if (Debug)
				log.WriteLog("Branch attivo: " + Branch);
			
			return Branch;
		}
		
		VectString GetBranches(Log& log, bool Debug)
		{
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git branch");
			VectString Branches;
			
			while (Out.find("  ") != string::npos)
			{
				Branches.push_back(SimpleFind(Out, "  ", "\n"));
				
				Out.erase(Out.find("  "), Out.find("\n") + 1);
			}
			
			if (Debug)
			{
				string BranchesStr = "";
				for (int i = 0; i < Branches.size(); i++)
					BranchesStr = BranchesStr + Branches[i] + ", ";
				log.WriteLog("Branches: " + BranchesStr);
			}
			
			return Branches;
		}
		
		short Commit(string CommitMessage, Log& log, bool Debug)
		{
			int Rtn;
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git commit -m \"" + CommitMessage + "\"", Rtn);
			
			if (Debug)
				log.WriteLog("Commit: " + to_string(Rtn) + "\n" + Out);
			
			if (Rtn == 0)
				return 0;
			else if (Rtn == 1)
			{
				if( Out.find("nothing to commit") != string::npos )
					return 1;
				else if (Out.find("Changes not staged for commit") != string::npos || Out.find("Untracked files") != string::npos)
					return 2;
				else if (Out.find("Aborting commit due to empty commit message.") != string::npos)
					return 3;
			}
			
			log.WriteLog("Errore durante il commit:\n" + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		short ChangeBranch(string BranchName, Log& log, bool Debug)
		{
			int Rtn;
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git checkout " + BranchName, Rtn);
			
			if (Debug)
				log.WriteLog("Cambio branch: " + to_string(Rtn) + "\n" + Out);
			
			if (Rtn == 0)
				return 0;
			else if (Rtn == 1)
			{
				if (Out.find("did not match any file(s) known to git") != string::npos)
					return 1; // Branch non trovato
				else if (Out.find("Please commit your changes or stash them before you switch branches.") != string::npos)
					return 2; // Ci sono dei cambiamenti senza commit
			}
			
			log.WriteLog("Errore durante il checkout del branch:\n" + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		short CreateBranch(string BranchName, Log& log, bool Debug)
		{
			int Rtn;
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git checkout -b " + BranchName, Rtn);
			
			if(Debug)
				log.WriteLog("Creazione branch: " + to_string(Rtn) + "\n" + Out);
			
			if(Rtn == 0)
				return 0;
			else if(Rtn == 128)
			{
				if(Out.find("already exists") != string::npos)
					return 1;
				else if(BranchName.find(" ") != string::npos)
					return 2; // BranchName contiene spazi
			}
			else if(Rtn == 129)
			{
				if(Out.find("error: switch `b' requires a value") != string::npos)
					return 3;
			}
			
			log.WriteLog("Errore durante la creazione del branch:\n" + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		short DeleteBranch(string BranchName, Log& log, bool Debug)
		{
			int Rtn;
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git branch -d " + BranchName, Rtn);
			
			if (Debug)
				log.WriteLog("Eliminazione branch: " + to_string(Rtn) + "\n" + Out);
			
			if (Rtn == 0)
				return 0;
			else if (Rtn == 1)
			{
				if (Out.find("is not fully merged.") != string::npos)
					return 1;
				else
					return 2; // Branch non trovato
			}
			
			log.WriteLog("Errore durante l'eliminazione del branch:\n" + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		void ConfirmDeleteBranch(string BranchName, Log& log, bool Debug)
		{
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git branch -D " + BranchName);
			
			if (Debug)
				log.WriteLog("Conferma eliminazione Branch " + BranchName + ":\n" + Out);
		}
		
		short Pull(Log& log, bool Debug)
		{
			CLInterface cli;
			int Rtn;
			
			cout << "Pull in corso ";
			cli.OneCharBar();
			
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git pull origin " + GetActiveBranch(log, Debug), Rtn);
			
			cli.StopBar(100);
			cout << endl;
			
			if (Debug)
				log.WriteLog("Pull: " + to_string(Rtn) + "\n" + Out);
			
			if (Rtn == 0)
			{
				if ( Out.find("Merge made by the") != string::npos || Out.find("Updating ") != string::npos )
					return 0;
				else if( Out.find("Already up-to-date.") != string::npos || Out.find("Already up to date.") != string::npos )
					return 1;
			}
			else if (Rtn == 1)
			{
				if ( Out.find("fatal: couldn't find remote ref") != string::npos)
					return 2;
				else if (Out.find("Automatic merge failed; fix conflicts and then commit the result.") != string::npos)
					return 3;
				else if (Out.find("fatal: 'origin' does not appear to be a git repository") != string::npos || Out.find("remote: Repository not found.") != string::npos)
					return 5;
			}
			else if (Rtn == 128)
			{
				if ( Out.find("error: Pulling is not possible because you have unmerged files.") != string::npos)
					return 4;
			}
			
			log.WriteLog("Errore durante il pull: " + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		short Push(Log& log, bool Debug)
		{
			int Rtn;
			CLInterface cli;
			
			cout << "Push in corso ";
			cli.OneCharBar();
			
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git push origin " + GetActiveBranch(log, Debug), Rtn);
			
			cli.StopBar(300);
			cout << endl;
			
			if (Debug)
				log.WriteLog("Push: " + to_string(Rtn) + "\n" + Out);
			
			if (Rtn == 0)
			{
				if ( Out.find("Enumerating objects:") != string::npos )
					return 0; // OK
				else if (Out.find("->") != string::npos && Out.find("To ") != string::npos && Out.find("..") != string::npos)
					return 0; // OK
				else if (Out.find("->") != string::npos && Out.find("[new branch]") != string::npos)
					return 0; // OK
				else if ( Out.find("Everything up-to-date") != string::npos )
					return 1; // Già aggiornato
			}
			else if (Rtn == 1)
			{
				if ( Out.find("does not match any") != string::npos)
					return 2; // Branch non trovato
				else if ( Out.find("You may want to first integrate the remote changes"))
					return 5; // Il branch remoto è più recente di quello locale
			}
			else if (Rtn == 128)
			{
				if ( Out.find("fatal: 'origin' does not appear to be a git repository") != string::npos || Out.find("remote: Repository not found.") != string::npos || Out.find("Host key verification failed.") != string::npos)
					return 3; // Bisogna aggiungere il link al repository remoto "git remote add origin <remote_URL>"
				else if ( Out.find("fatal: No configured push destination.") != string::npos)
					return 4; // Nessuna destinazione configurata per il push
			}
			
			log.WriteLog("Errore durante il push:\n" + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		short Stage(Log& log, bool Debug)
		{
			int Rtn;
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git add .", Rtn);
			
			if (Debug)
				log.WriteLog("Stage: " + to_string(Rtn) + "\n" + Out);
			
			if (Rtn == 0)
				return 0;
			
			log.WriteLog("Errore durante lo stage:\n" + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		string GetStatus(Log& log, bool Debug)
		{
			du.ChangeCurrDir(RepoPath);
			TextColor tc;
			
			string Out = gu.GetCMDOutput("git status");
			
			if(Debug)
				log.WriteLog("Git Status: " + Out);
			
			if(Out.find("Changes not staged for commit:") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "Cambiamenti senza Stage";
			}
			if (Out.find("Untracked files:") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "File non tracciati, atteso Stage";
			}
			if(Out.find("Changes to be committed:") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "Atteso Commit";
			}
			if(Out.find("Your branch is behind") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "Atteso Pull";
			}
			if(Out.find("Your branch is ahead of") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "Push atteso per " + SimpleFind(Out, "by ", " commit") + " Commit";
			}
			if(Out.find("You have unmerged paths.") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "Merge in corso";
			}
			if(Out.find("All conflicts fixed but you are still merging.") != string::npos)
			{
				tc.SetColor(tc.Yellow);
				return "Atteso Commit";
			}
			if(Out.find("Your branch is up to date with") != string::npos || Out.find("nothing to commit, working tree clean") != string::npos)
			{
				tc.SetColor(tc.Green);
				return "OK";
			}
			
			tc.SetColor(tc.Red);
			return "NON DEFINITO";
		}
		
		short Fetch(Log& log, bool Debug, bool InsideCmd = true)
		{
			CLInterface cli;
			ConsoleUtils cu;
			
			int Rtn;
			COORD pos = cu.GetCursorPos();
			
			du.ChangeCurrDir(RepoPath);
			
			if(InsideCmd)
				cout << "Fetch in corso ";
			else
				cout << "\nCaricamento in corso ";
			
			cli.OneCharBar();
			
			string Out = gu.GetCMDOutput("git fetch origin", Rtn);
			
			cli.StopBar(100);
			cout << endl;
			
			if(Debug)
				log.WriteLog("Git Fetch: " + to_string(Rtn) + "\n" + Out);
			
			if(Rtn == 0)
				return 0;
			else if(Rtn == 128)
				return 1;
			
			log.WriteLog("Errore durante il Fetch: " + to_string(Rtn) + "\n" + Out);
			return -1;
		}

		short Stash(Log& log, bool Debug)
		{
			int Rtn;
			du.ChangeCurrDir(RepoPath);
			string Out = gu.GetCMDOutput("git stash", Rtn);

			if (Debug)
				log.WriteLog("Stash salvato: " + to_string(Rtn) + "\n" + Out);

			Out = gu.GetCMDOutput("git stash drop", Rtn);

			if (Debug)
				log.WriteLog("Stash droppato: " + to_string(Rtn) + "\n" + Out);

			if (Rtn == 0)
				return 0;
			else if (Rtn == 1)
			{
				if (Out.find("No stash entries found.") != string::npos)
					return 1;
			}

			log.WriteLog("Errore durante lo Stash: " + to_string(Rtn) + "\n" + Out);
			return -1;
		}
		
		bool IsRepo()
		{
			if(!du.ChangeCurrDir(RepoPath))
				return false;
			
			if (gu.NoOutputCMD("git rev-parse --is-inside-work-tree") == 0)
				return true;
			
			return false;
		}
		
		void StartStatusCheck(Log& log, bool Debug, COORD CursorPos)
		{
			Stop = false;
			thread t(&GitProject::SC, this, ref(log), Debug, CursorPos);
			t.detach();
		}
		
		void StopStatusCheck()
		{
			Stop = true;
			Sleep(100);
		}
};
