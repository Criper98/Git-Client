#include <iostream>
#include <Essentials.h>
#include <EssCurl.h>
#include "Classi.h"

using namespace std;

typedef vector<GitProject> GitProjects;

#include "Funzioni.h"

int main()
{
	DirUtils du;
	TextColor tc;
	EasyMSGB msgb;
	SystemUtils su;
	ConsoleUtils cu;
	CLInterface cli;
	WindowUtils wu;
	Log log( du.GetModuleFilePath() + "log.txt" );
	SettingsFile sf( du.GetModuleFilePath() + "settings.ini" );
	
	cout << "Caricamento in corso..." << endl;
	cli.FullBar(30);
	
	bool Debug = false;
	string Version = "1.1.5";
	GitProjects Projects;
	VectString MenuPrincipale;
	VectString MenuRepo;
	VectString RepoNames;
	VectSettings Settings;
	
	cli.LoadingPercentage = 12;
	
	cu.ConsoleTitle("Git Client By Criper98");
	cu.ExtendAsciiOutput();
	log.RWFile();
	msgb.DefTitle = "Git Client";
	
	cli.LoadingPercentage = 25;
	
	MenuPrincipale.push_back("Esci");
	MenuPrincipale.push_back("Apri Repo");
	MenuPrincipale.push_back("Aggiungi Repo");
	MenuPrincipale.push_back("Clona Repo");
	MenuPrincipale.push_back("Crea Repo");
	MenuPrincipale.push_back("Rimuovi Repo");
	MenuPrincipale.push_back("Impostazioni");
	
	cli.LoadingPercentage = 50;
	
	MenuRepo.push_back("Torna Indietro");
	MenuRepo.push_back("Stage");
	MenuRepo.push_back("Commit");
	MenuRepo.push_back("Push");
	MenuRepo.push_back("Cambia Branch");
	MenuRepo.push_back("Crea Branch");
	MenuRepo.push_back("Elimina Branch");
	MenuRepo.push_back("Pull");
	MenuRepo.push_back("Fetch");
	MenuRepo.push_back("Stash");
	
	cli.LoadingPercentage = 62;
	
	Settings = LoadSettings(sf);
	if (Settings[1].Value == "true")
		Debug = true;
	
	cli.LoadingPercentage = 75;
	
	if (!Settings[2].Value.empty())
		wu.SetWindowSize({ (short)stoi(Settings[2].Value), (short)stoi(Settings[2].SecValue) });
	else
		msgb.Ok("Errore durante il caricamento dei settaggi.", msgb.Error);
	
	cli.LoadingPercentage = 100;
	cli.StopBar(250);

	if (Settings[3].Value == "true")
		if (CheckUpdate(log, Debug, Version))
			return 0;
	
	for(bool i=true; i;)
	{
		system("cls");
		StampaTitolo(Version, 1);
		cli.SubTitle("Menu Principale", 50);
		int Scelta = cli.MenuSingleSelQuadre(MenuPrincipale);
		
		// Esci
		if(Scelta == 0)
			i = false;
		
		// Apri un Repo
		if (Scelta == 1)
		{
			RepoNames = LoadSavedRepo(Projects, log, Debug);
			
			if (RepoNames.size() > 0)
			{
				cout << "\nLista dei Repository:" << endl;
				int ActiveRepo = cli.MenuSingleSelScorrimento(RepoNames, tc.Purple);
				
				if(Projects[ActiveRepo].IsRepo())
				{
					bool StatusCheck = true;

					UpdateRepoList(Projects, ActiveRepo);
					
					Projects[ActiveRepo].Fetch(log, Debug, false);
					
					while(i)
					{
						system("cls");
						StampaTitolo(Version, 1);
						cli.SubTitle("Menu Repository", 50);
						
						cout << "Percorso Repo:\t[ "; tc.SetColor(tc.Green);
						cout << Projects[ActiveRepo].RepoPath; tc.SetColor(tc.Default);
						cout << " ]" << endl;
						
						cout << "Nome Repo:\t[ "; tc.SetColor(tc.Green);
						cout << Projects[ActiveRepo].GetRepoName(log, Debug); tc.SetColor(tc.Default);
						cout << " ]" << endl;
						
						cout << "Branch Attivo:\t[ "; tc.SetColor(tc.Green);
						cout << Projects[ActiveRepo].GetActiveBranch(log, Debug); tc.SetColor(tc.Default);
						cout << " ]" << endl;
						
						cout << "Status:\t\t[ ";
						if(StatusCheck)
						{
							Projects[ActiveRepo].StartStatusCheck(log, Debug, cu.GetCursorPos(), wu.hwnd);
							StatusCheck = false;
						}
						cout << Projects[ActiveRepo].GetStatus(log, Debug); tc.SetColor(tc.Default);
						cout << " ]" << endl << endl;
						
						Scelta = cli.MenuSingleSelQuadre(MenuRepo);
						
						cout << endl;
						
						// Torna Indietro
						if (Scelta == 0)
							i = false;
						
						// Stage
						if (Scelta == 1)
						{
							switch (Projects[ActiveRepo].Stage(log, Debug))
							{
								case 0:
									tc.SetColor(tc.Lime);
									cout << "Stage eseguito nel Branch " << Projects[ActiveRepo].GetActiveBranch(log, Debug) << endl;
								break;
								
								default:
									tc.SetColor(tc.Red);
									cout << "Errore sconosciuto." << endl;
									Sleep(1000);
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Commit
						if (Scelta == 2)
						{
							string CommitMessage;
							cout << "Inserisci il messaggio del Commit: ";
							getline(cin, CommitMessage);
							
							switch (Projects[ActiveRepo].Commit(CommitMessage, log, Debug))
							{
								case 0:
									tc.SetColor(tc.Lime);
									cout << "Commit effettuato con successo!" << endl;
								break;
								
								case 1:
									tc.SetColor(tc.Purple);
									cout << "Nessun Commit da effettuare." << endl;
									Sleep(1000);
								break;
								
								case 2:
									tc.SetColor(tc.Yellow);
									cout << "Prima devi eseguire lo Stage dei cambiamenti." << endl;
									Sleep(1000);
								break;
								
								case 3:
									tc.SetColor(tc.Yellow);
									cout << "Il messaggio del Commit non può essere vuoto." << endl;
									Sleep(1000);
								break;
								
								default:
									tc.SetColor(tc.Red);
									cout << "Errore sconosciuto." << endl;
									Sleep(1000);
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Push
						if (Scelta == 3)
						{
							switch (Projects[ActiveRepo].Push(log, Debug))
							{
								case 0:
									tc.SetColor(tc.Lime);
									cout << "Push eseguito nel Branch " << Projects[ActiveRepo].GetActiveBranch(log, Debug) << endl;
								break;
								
								case 1:
									tc.SetColor(tc.Purple);
									cout << "Nessun cambiamento da applicare." << endl;
									Sleep(1000);
								break;
								
								case 2:
									tc.SetColor(tc.Yellow);
									cout << "Il Branch " << Projects[ActiveRepo].GetActiveBranch(log, Debug) << " non esiste nel Repository remoto." << endl;
									Sleep(1000);
								break;
								
								case 3:
									AssociateRemoteRepo(log, Debug, Projects, ActiveRepo);
									Sleep(1000);
								break;
								
								case 4:
									tc.SetColor(tc.Yellow);
									cout << "Nessuna destinazione configurata per il push." << endl;
									Sleep(1000);
								break;
								
								case 5:
									tc.SetColor(tc.Yellow);
									cout << "Il branch remoto è più aggiornato di quello locale.\nProva a fare un Pull." << endl;
									Sleep(2000);
								break;
								
								default:
									tc.SetColor(tc.Red);
									cout << "Errore sconosciuto." << endl;
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Cambia Branch
						if (Scelta == 4)
						{
							VectString Branches = Projects[ActiveRepo].GetBranches(log, Debug);
							
							if( Branches.size() > 0 )
							{
								cout << "Seleziona il branch:" << endl;
								
								switch ( Projects[ActiveRepo].ChangeBranch( Branches[cli.MenuSingleSelScorrimento( Branches, tc.Purple )], log, Debug) )
								{
									case 0:
										tc.SetColor(tc.Lime);
										cout << "Branch cambiato con successo!" << endl;
									break;
									
									case 1:
										tc.SetColor(tc.Yellow);
										cout << "Branch non esistente." << endl;
										Sleep(1000);
									break;
									
									case 2:
										tc.SetColor(tc.Yellow);
										cout << "Ci sono dei cambiamenti senza Commit." << endl;
										Sleep(1000);
									break;
									
									default:
										tc.SetColor(tc.Red);
										cout << "Errore sconosciuto." << endl;
										Sleep(1000);
								}
							}
							else
							{
								tc.SetColor(tc.Red);
								cout << "Nessun Branch aggiuntivo presente." << endl;
								Sleep(1000);
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Crea Branch
						if (Scelta == 5)
						{
							string BranchName;
							cout << "Inserisci il nome del nuovo Branch: ";
							getline(cin, BranchName);
							
							switch ( Projects[ActiveRepo].CreateBranch(BranchName, log, Debug) )
							{
								case 0:
									tc.SetColor(tc.Lime);
									cout << "Branch creato con successo!" << endl;
								break;
								
								case 1:
									tc.SetColor(tc.Yellow);
									cout << "Branch già esistente." << endl;
									Sleep(1000);
								break;
								
								case 2:
									tc.SetColor(tc.Yellow);
									cout << "Il nome indicato non deve contenere spazi." << endl;
									Sleep(1000);
								break;
								
								case 3:
									tc.SetColor(tc.Yellow);
									cout << "Il nome non può essere vuoto." << endl;
									Sleep(1000);
								break;
								
								default:
									tc.SetColor(tc.Red);
									cout << "Errore sconosciuto." << endl;
									Sleep(1000);
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Elimina Branch
						if (Scelta == 6)
						{
							VectString Branches = Projects[ActiveRepo].GetBranches(log, Debug);
							Branches.push_back("[ ANNULLA ]");
							
							if( Branches.size() > 1 )
							{
								cout << "Seleziona il branch:" << endl;
								
								int Sel = cli.MenuSingleSelScorrimento(Branches, tc.Red);
								
								cout << endl;
								
								if(Branches[Sel] != "[ ANNULLA ]")
									switch ( Projects[ActiveRepo].DeleteBranch( Branches[Sel], log, Debug) )
									{
										case 0:
											tc.SetColor(tc.Lime);
											cout << "Branch eliminato con successo!" << endl;
										break;
										
										case 1:
											tc.SetColor(tc.Yellow);
											cout << "ATTENZIONE: il branch selezionato ha dei cambiamenti non presenti in quello principale!" << endl;
											cout << "Sei sicuro di volerlo eliminare?" << endl;
											if (cli.MenuSingleSelQuadre({ "Si", "No" }) == 0)
											{
												Projects[ActiveRepo].ConfirmDeleteBranch(Branches[Sel], log, Debug);
												tc.SetColor(tc.Lime);
												cout << "\nBranch eliminato con successo!" << endl;
											}
											else
											{
												tc.SetColor(tc.Purple);
												cout << "\nOperazione annullata." << endl;
												Sleep(1000);
											}
										break;
										
										case 2:
											tc.SetColor(tc.Yellow);
											cout << "Il Branch selezionato non esiste." << endl;
											Sleep(1000);
										break;
										
										default:
											tc.SetColor(tc.Red);
											cout << "Errore sconosciuto." << endl;
											Sleep(1000);
									}
							}
							else
							{
								tc.SetColor(tc.Red);
								cout << "Nessun Branch aggiuntivo presente." << endl;
								Sleep(1000);
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Pull
						if (Scelta == 7)
						{
							switch (Projects[ActiveRepo].Pull(log, Debug))
							{
								case 0:
									tc.SetColor(tc.Lime);
									cout << "Pull eseguito sul Branch " << Projects[ActiveRepo].GetActiveBranch(log, Debug) << endl;
								break;
								
								case 1:
									tc.SetColor(tc.Purple);
									cout << "Branch " << Projects[ActiveRepo].GetActiveBranch(log, Debug) << " già aggiornato." << endl;
									Sleep(1000);
								break;
								
								case 2:
									tc.SetColor(tc.Yellow);
									cout << "Il Branch " << Projects[ActiveRepo].GetActiveBranch(log, Debug) << " non esiste nel Repository remoto." << endl;
									Sleep(1000);
								break;
								
								case 3:
									tc.SetColor(tc.Yellow);
									cout << "Merge automatico fallito, risolvi i conflitti, esegui un Commit e ritenta." << endl;
									Sleep(1000);
								break;
								
								case 4:
									tc.SetColor(tc.Yellow);
									cout << "Pull non eseguito, Merge in corso, risolvi i conflitti ed esegui un Commit." << endl;
									Sleep(2000);
								break;
								
								case 5:
									AssociateRemoteRepo(log, Debug, Projects, ActiveRepo);
									Sleep(2000);
								break;
								
								default:
									tc.SetColor(tc.Red);
									cout << "Errore sconosciuto." << endl;
									Sleep(1000);
							}
							
							tc.SetColor(tc.Default);
							Sleep(1000);
						}
						
						// Fetch
						if (Scelta == 8)
						{
							switch(Projects[ActiveRepo].Fetch(log, Debug))
							{
								case 0:
									tc.SetColor(tc.Lime);
									cout << "Fetch eseguito." << endl;
								break;
								
								case 1:
									AssociateRemoteRepo(log, Debug, Projects, ActiveRepo);
									Sleep(1000);
								break;
								
								default:
									tc.SetColor(tc.Red);
									cout << "Errore sconosciuto." << endl;
									Sleep(1000);
							}
							
							Sleep(1000);
						}

						// Stash
						if (Scelta == 9)
						{
							cout << "Sei sicuro di voler annullare tutti i cambiamenti senza commit?" << endl;

							if (cli.MenuSingleSelQuadre({ "Si", "No" }) == 0)
								switch (Projects[ActiveRepo].Stash(log, Debug))
								{
									case 0:
										tc.SetColor(tc.Lime);
										cout << "Cambiamenti eliminati." << endl;
									break;

									case 1:
										tc.SetColor(tc.Yellow);
										cout << "Nessun cambiamento da eliminare." << endl;
										Sleep(1000);
									break;

									default:
										tc.SetColor(tc.Red);
										cout << "Errore sconosciuto." << endl;
										Sleep(1000);
								}
							else
								cout << "Operazione annullata." << endl;

							Sleep(1000);
						}
					}
					
					Projects[ActiveRepo].StopStatusCheck();
					du.SetCurrDir(du.GetModuleFilePath());
				}
				else
				{
					tc.SetColor(tc.Yellow);
					cout << "\nRepo non trovato.\nPercorso: [" << Projects[ActiveRepo].RepoPath << "]\nRimuovere il Repo dalla lista?" << endl;
					tc.SetColor(tc.Default);
					if(cli.MenuSingleSelQuadre({ "Si", "No" }) == 0)
					{
						RemoveRepo(Projects, ActiveRepo);
						tc.SetColor(tc.Green);
						cout << "Repo rimosso." << endl;
						tc.SetColor(tc.Default);
					}
					Sleep(1000);
				}
				
				Scelta = -1;
				i = true;
			}
			else
			{
				tc.SetColor(tc.Yellow);
				cout << "\nNon hai Repo salvati" << endl;
				Sleep(2000);
			}
			
			tc.SetColor(tc.Default);
		}
		
		// Aggiungi un Repo
		if (Scelta == 2)
		{
			string Path = du.ChoseDirDialog();
			
			if (!Path.empty())
			{
				if (SaveRepoPath(Path, Debug, log))
				{
					tc.SetColor(tc.Lime);
					cout << "\nRepo salvato" << endl;
				}
				else
				{
					tc.SetColor(tc.Red);
					cout << "\nSalvataggio fallito: Probabilmente il percorso selezionato non contiene un Repo." << endl;
					Sleep(2000);
				}
				
				Sleep(1000);
				tc.SetColor(tc.Default);
			}
		}
		
		// Clona un Repo
		if (Scelta == 3)
		{
			string URL;
			string Path;
			string CMDout;
			string FullPath;
			int ReturnCode = -1;
			
			cout << "\nURL del Repo: ";
			cin >> URL;
			cin.ignore();
			
			cout << "Percorso di salvataggio: ";
			Path = du.ChoseDirDialog();
			cout << Path << endl;
			
			if (!Path.empty())
			{
				cout << "\nClone in corso ";
				cli.OneCharBar();
				
				du.SetCurrDir(Path);
				CMDout = su.GetCMDOutput("git clone " + URL, ReturnCode);
				
				cli.StopBar(100);
			}
			
			if(Debug)
				log.WriteLog("\nGit Clone: " + to_string(ReturnCode) + "\n" + CMDout);
			
			if (ReturnCode == 0)
			{
				tc.SetColor(tc.Lime);
				cout << "\nRepo clonato." << endl;
				
				FullPath = Path + "\\" + URL.substr(URL.find_last_of('/') + 1);
				FullPath.erase(FullPath.size() - 4, 4);
				
				SaveRepoPath(FullPath, Debug, log);
			}
			else if(ReturnCode == 128)
			{
				if (CMDout.find("already exists and is not an empty directory.") != string::npos)
				{
					tc.SetColor(tc.Red);
					cout << "Il percorso indicato non è vuoto." << endl;
				}
				else if (CMDout.find("remote: Repository not found.") != string::npos)
				{
					tc.SetColor(tc.Red);
					cout << "Repository remoto non trovato." << endl;
				}
				else
				{
					tc.SetColor(tc.Red);
					cout << "Errore sconosciuto." << endl;
				}
			}
			else if(ReturnCode == -1)
			{
				tc.SetColor(tc.Purple);
				cout << "Operazione annullata." << endl;
			}
			
			tc.SetColor(tc.Default);
			
			Sleep(2000);
		}
		
		// Crea un Repo
		if (Scelta == 4)
		{
			string Path;
			
			cout << "\nScegli il percorso: ";
			Path = du.ChoseDirDialog();
			cout << Path << endl;
			
			if(!Path.empty())
			{
				switch(CreateRepo(log, Debug, Path))
				{
					case 0:
						SaveRepoPath(Path, Debug, log);
						tc.SetColor(tc.Lime);
						cout << "Repo creato e aggiunto alla lista." << endl;
						Sleep(1000);
					break;
					
					default:
						tc.SetColor(tc.Red);
						cout << "Errore sconosciuto." << endl;
						Sleep(1000);
					break;
				}
			}
			else
			{
				tc.SetColor(tc.Purple);
				cout << "Operazione annullata." << endl;
			}
			
			tc.SetColor(tc.Default);
			Sleep(1000);
		}
		
		// Rimuovi un Repo
		if (Scelta == 5)
		{
			RepoNames = LoadSavedRepo(Projects, log, Debug);
			
			if (RepoNames.size() > 0)
			{
				cout << "\nScegli il repo da rimuovere:" << endl;
				RepoNames.push_back("[ ANNULLA ]");
				int Selection = cli.MenuSingleSelScorrimento(RepoNames, tc.Bordeaux);
				
				if(RepoNames[Selection] != "[ ANNULLA ]")
				{
					cout << "\nSei sicuro di voler rimuovere " << RepoNames[Selection] << " dai Repository salvati?" << endl;
					if(cli.MenuSingleSelQuadre({"Si", "No"}) == 0)
					{
						RemoveRepo(Projects, Selection);
						tc.SetColor(tc.Lime);
						cout << "\nRepo rimosso!" << endl;
					}
				}
				RepoNames = LoadSavedRepo(Projects, log, Debug);
				Sleep(1000);
			}
			
			else
			{
				tc.SetColor(tc.Red);
				cout << "\nNon hai Repo salvati" << endl;
				Sleep(2000);
			}
			
			tc.SetColor(tc.Default);
		}
		
		// Impostazioni
		if (Scelta == 6)
		{
			while (i)
			{
				system("cls");
				StampaTitolo(Version, 1);
				cli.SubTitle("Impostazioni", 50);
				
				Settings = LoadSettings(sf);
				
				Scelta = cli.MenuSettings(Settings);
				
				if (Scelta == 0)
					i = false;
				else if (Scelta == 1)
				{
					if (Settings[1].Value == "true")
						sf.SetSetting("Debug", "false");
					else
						sf.SetSetting("Debug", "true");
					
					if (sf.GetSetting("Debug") == "true")
						Debug = true;
					else
						Debug = false;
				}
				else if (Scelta == 2)
				{
					cout << "\nRidimensiona console a piacimento e premi invio..." << endl;
					_getch();
					
					sf.SetSetting("ConsoleSizeX", to_string((int)wu.GetWindowSize().X));
					sf.SetSetting("ConsoleSizeY", to_string((int)wu.GetWindowSize().Y));
				}
				else if (Scelta == 3)
				{
					if (Settings[3].Value == "true")
						sf.SetSetting("CheckUpdate", "false");
					else
						sf.SetSetting("CheckUpdate", "true");
				}
			}
			
			i = true;
			Scelta = -1;
		}
	}
	
	return 0;
}