#define WINVER  0x0600 //requiere  windows ver 6.xx et +

#if WINVER <0x0403 
#define TTM_SETTITLE 0x420
#define CCM_SETWINDOWTHEME 0x200B
#define TTS_BALLOON	0x40
#endif
#ifdef WINVER
#pragma warning(disable:4996)
#endif // WINVER

#define IDM_TIMER1 0x01000
#define WM_TRAY_ICONE WM_USER + 101
#include <windows.h> //entetes requises par l'application
#include <windowsx.h>
#include <wingdi.h>
#include <commctrl.h>
#include <Winuser.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <conio.h>
#include "resource.h"
//indique au compilateur d'inclure la librairie
#pragma comment (lib,"comctl32.lib") 
#pragma comment (lib,"kernel32.lib") 
#pragma comment (lib,"user32.lib") 
#pragma comment (lib,"gdi32.lib") 
#pragma comment (lib,"winspool.lib") 
#pragma comment (lib,"comdlg32.lib") 
#pragma comment (lib,"advapi32.lib") 
#pragma comment (lib,"shell32.lib") 
#pragma comment (lib,"ole32.lib") 
#pragma comment (lib,"oleaut32.lib") 
#pragma comment (lib,"uuid.lib") 
#pragma comment (lib,"odbc32.lib") 
#pragma comment (lib,"odbccp32.lib") 

//mappage des classes
HINSTANCE hInst;
BROWSEINFO bi;
ITEMIDLIST *il;
OPENFILENAME ofn;
char szPath[MAX_PATH];
char appPath[MAX_PATH];
char buffer[MAX_PATH];
char Result[MAX_PATH];
char szFile[MAX_PATH];
INITCOMMONCONTROLSEX iccex; 
WNDCLASS wc;
HWND hWnd,hwndTT,imghWnd;
NOTIFYICONDATAA nf;
TOOLINFO ti;
RECT rc,rect;
HBITMAP bmp;
HMENU mnu;
SYSTEMTIME st;
BOOL AffichageMenu=FALSE;
//Tableaux
char jours[7][10] = {"dimanche", "lundi","mardi","mercredi","jeudi","vendredi","samedi"};
char mois[12][10] = {"janvier", "février","mars", "avril", "mai", "juin","juillet","aout","septembre", "octobre", "novembre", "décembre"};

void CreateMyTooltip(HWND hDlg,const char* ToolTipTitre);
void CreateMyTooltip (HWND hDlg,char *ToolTipTitre);
void ActiveToolTips();
static BOOL CALLBACK DialogFunc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow);
VOID APIENTRY DisplayContextMenu(HWND hDlg,POINT pt);
BOOL WINAPI OnContextMenu(int x, int y) ;
void AjouterAuMenu(HMENU menu);
OPENFILENAME CommunFichiers(LPCTSTR Titre,LPCTSTR Filtre);
void OuvrirFichier(LPCTSTR Titre);
void SauverFichier(LPCTSTR Titre);
void ExplorerDossiers(LPCTSTR Titre);

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow){
	hInst=hinst; //formward l'instance
	iccex.dwICC=ICC_WIN95_CLASSES;
	iccex.dwSize=sizeof(iccex);
	InitCommonControlsEx(&iccex);
	GetCurrentDirectory(0xFF,appPath);
	memset(&wc,0,sizeof(wc));
	wc.hCursor=LoadCursor(hinst, (LPCTSTR)IDC_CURSOR1);
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.lpszClassName ="ColorDLG";
	wc.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0,0,128));//couleur de fond
	wc.hIcon=LoadIcon(hinst,(LPCTSTR)IDI_ICON1);
	wc.style = CS_VREDRAW  | CS_HREDRAW | CS_SAVEBITS | CS_DBLCLKS;
	RegisterClass(&wc);
	return DialogBox(hinst, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC) DialogFunc);
}

static BOOL CALLBACK DialogFunc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	hWnd =hDlg; //forward le hDlg pour les autres modules (simplifie les fonctions)
	POINT mouse;
	switch (msg) {
	case WM_INITDIALOG:
//instruction de lancement 
		SetTimer(hDlg,IDM_TIMER1,1000,(TIMERPROC) NULL); //crrer le timer avec interval de 1000 ms = 1s
		//modification du menu systeme
		RemoveMenu(GetSystemMenu(hDlg,FALSE),SC_CLOSE,MF_STRING);  //deactive le X du dialogue
		AppendMenu(GetSystemMenu(hDlg,FALSE),MF_SEPARATOR,MF_STRING,"");//séparateur
		AjouterAuMenu(GetSystemMenu(hDlg,FALSE)); //ajoute les commandes communes au menu
		AppendMenu(GetSystemMenu(hDlg,FALSE),MF_SEPARATOR,MF_STRING,"");
		AppendMenu(GetSystemMenu(hDlg,FALSE),MF_STRING,2,"Fermer ce programme");
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_MOVE);
	 	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),SC_MOVE,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER);
	 	SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),IDCANCEL,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ABOUT);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE140,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BROWSE);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),IDC_BUTTON1,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_EXECUTE);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE141,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ADD);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE142,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_CLEAR);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE143,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_OPEN);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE144,MF_BYCOMMAND,bmp,bmp);
		bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
		SetMenuItemBitmaps(GetSystemMenu(hDlg,FALSE),0xE145,MF_BYCOMMAND,bmp,bmp);
		//fin des modifs menu systeme
		SendMessage(GetDlgItem(hDlg,IDCANCEL), WM_SETFONT, (WPARAM)GetStockObject(0x1E), MAKELPARAM(TRUE, 0)); //aplique la police au bouton designe
		SendMessage(GetDlgItem(hDlg,IDOK), WM_SETFONT, (WPARAM)GetStockObject(0x1F), MAKELPARAM(TRUE, 0));
		SendMessage(GetDlgItem(hDlg,0xE140), STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON1));
		nf.cbSize=sizeof(nf); // definit la taille pour le systray
		nf.hIcon=wc.hIcon; // définit l'icone primaire 
		nf.hWnd=hDlg; //definition du Handle propriétaire
		strcpy(nf.szTip,"Démo pour étude © Papy67 2005\0");// requiere un string 0 max 64 car si < NT 5.xx sinon 128 car
		nf.uCallbackMessage=WM_TRAY_ICONE; // a remplacer par la procédure de gestion s'il y a 
		nf.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP; //definit les drapeaux de taches
		Shell_NotifyIcon(NIM_ADD,&nf);//icone dans le traysys
		//création de l'icone Logo
		imghWnd = CreateWindowEx(0, "STATIC", NULL, WS_VISIBLE|WS_CHILD|SS_ICON,1, 1, 10, 10,hDlg , (HMENU)45000, wc.hInstance, NULL);
		SendMessage(imghWnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance,(LPCTSTR)IDI_ICON2));
		SetWindowText(hDlg,strupr(__argv[0])); //titre du dialogue
		GetLocalTime(&st); //requete  dahte et heure PC
		sprintf(buffer,"Nous sommes %s, le %2d %s %4d",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear); //creation du string de date
		SetDlgItemText(hDlg,IDC_DATE,buffer);
		return TRUE;
	case WM_SYSCOMMAND:// instruction des commandes menu systeme
		switch (LOWORD(wParam)) {
			case IDC_BUTTON1:
				ExplorerDossiers("Démo pour étude\nFunction BrowseForFolders");
				break;
			case 0xE140:ShellAbout(hDlg,"Démo pour étude",__argv[0],wc.hIcon);
				break;
			case 0xE141:ShellExecute(hDlg,"open",appPath,NULL,NULL,0x01);
				break;
			case 0xE142:	
				SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING, 0,(LPARAM)"Founction AddToList");//ajout a la liste
				break;
			case 0xE143:
				SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_RESETCONTENT, 0,0); //efface la liste
				MessageBox(hDlg,"Liste effacée","Démo pour étude",MB_OK|MB_ICONEXCLAMATION);
				break;
			case 0xE144:
				OuvrirFichier("Function GetOpenFileName");
				break;
			case 0xE145:
				SauverFichier("Function GetSaveFileName");
				break;
			case IDCANCEL:
				Shell_NotifyIcon(NIM_DELETE,&nf); 
				KillTimer(hDlg,IDM_TIMER1);
				PostQuitMessage(0);
				return 1;
		}
	   break;
	case WM_TRAY_ICONE:{ // pour l'affichage menu
		// seulement si double clic gauche ou clic droit
		GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0);
		switch(lParam){
			case WM_RBUTTONUP:
					GetCursorPos(&mouse);
					mnu=GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0);
					AppendMenu(mnu,MF_STRING|MF_GRAYED,SC_CLOSE,"Déplacer");
					AppendMenu(mnu,MF_SEPARATOR,MF_STRING,"");//séparateur
					AjouterAuMenu(mnu);
					SetMenuDefaultItem(GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0), 0xE140, FALSE);
					SetForegroundWindow(hDlg);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER); //charge l'image
				 	SetMenuItemBitmaps(mnu,IDCANCEL,MF_BYCOMMAND,bmp,bmp); //l'applique au menu choisi
				 	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ABOUT);
					SetMenuItemBitmaps(mnu,0xE140,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BROWSE);
					SetMenuItemBitmaps(mnu,IDC_BUTTON1,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_EXECUTE);
					SetMenuItemBitmaps(mnu,0xE141,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ADD);
					SetMenuItemBitmaps(mnu,0xE142,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_CLEAR);
					SetMenuItemBitmaps(mnu,0xE143,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_OPEN);
					SetMenuItemBitmaps(mnu,0xE144,MF_BYCOMMAND,bmp,bmp);
					bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
					SetMenuItemBitmaps(mnu,0xE145,MF_BYCOMMAND,bmp,bmp);
					TrackPopupMenu(mnu, TPM_RETURNCMD || TPM_RIGHTALIGN, mouse.x, mouse.y, 0, hDlg, NULL);
					DestroyMenu(GetSubMenu(LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU1)), 0));
					break;
			case WM_LBUTTONUP:
					GetCursorPos(&mouse);
					if (OnContextMenu( mouse.x,mouse.y)==FALSE){
						return DefWindowProc(hDlg, msg, wParam, lParam);
					}else{
						MessageBox(0,"Il n'y a pas de menu installé pour le clic Gauche",NULL,0);
					}
					break;
			}
		}
	case WM_COMMAND:// instruction des commandes menu et boutons etc...
		switch (LOWORD(wParam)) {
			case IDC_BUTTON1:
				ExplorerDossiers("Démo pour étude\nFunction BrowseForFolders");
				break;
			case 0xE140:ShellAbout(hDlg,"Démo pour étude",__argv[0],wc.hIcon);
				break;
			case 0xE141:ShellExecute(hDlg,"open",appPath,NULL,NULL,0x01);
				break;
			case 0xE142:	
				SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_ADDSTRING, 0,(LPARAM)"Founction AddToList");//ajout a la liste
				break;
			case 0xE143:
				SendMessage(GetDlgItem(hDlg,IDC_LIST1),LB_RESETCONTENT, 0,0); //efface la liste
				MessageBox(hDlg,"Liste effacée","Démo pour étude",MB_OK|MB_ICONEXCLAMATION);
				break;
			case 0xE144:
				OuvrirFichier("Function GetOpenFileName");
				break;
			case 0xE145:
				SauverFichier("Function GetSaveFileName");
				break;
			case IDCANCEL:
				Shell_NotifyIcon(NIM_DELETE,&nf); 
				KillTimer(hDlg,IDM_TIMER1);
				PostQuitMessage(0);
				return 1;
		}
	   break;
	case WM_TIMER:
		GetLocalTime(&st);
		sprintf(buffer,"Nous sommes %s, le %2d %s %4d",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth],st.wYear);
		SetDlgItemText(hDlg,IDC_DATE,buffer);
		sprintf(buffer,"%.2d:%.2d:%.2d",st.wHour,st.wMinute,st.wSecond);
		SetDlgItemText(hDlg,IDC_TIME,buffer);
	case WM_MOUSEMOVE:
			ActiveToolTips();
			break;
	case WM_CONTEXTMENU:
		//permet de metre le menu cotextuel return DefWindowProc(hDlg, msg, wParam, lParam);
			if (OnContextMenu( LOWORD(lParam),HIWORD(lParam))==FALSE)
				return DefWindowProc(hDlg, msg, wParam, lParam);
			break;
	case WM_CTLCOLORDLG:
		return (long)wc.hbrBackground;
		break;
	//les element statics
	case WM_CTLCOLORSTATIC:
	//couleur du texte
		SetTextColor( (HDC)wParam, RGB(0,255, 255) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)wc.hbrBackground; //retourne la couleur choisie dans la classe
	case WM_CTLCOLOREDIT: //Edit box
		SetTextColor( (HDC)wParam, RGB(0xFC,0xFF,0x00 ) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)(HBRUSH)CreateSolidBrush(RGB(0xCE,0x8C,0xFA));//couleur de fond
	case WM_CTLCOLORLISTBOX: //ListBox
		SetTextColor( (HDC)wParam, RGB(0,255, 255) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)(HBRUSH)CreateSolidBrush(RGB(0,128,0));//couleur de fond
	case WM_CTLCOLORBTN: //Bouton (owner draw doit etre définit)
		SetTextColor( (HDC)wParam, RGB(0,255, 255) );
		SetBkMode( (HDC)wParam, TRANSPARENT ); 
		return (LONG)(HBRUSH)CreateSolidBrush(RGB(128,0,0));//couleur de fond
	case WM_CTLCOLORMSGBOX: //dialogue
		SetTextColor( (HDC)wParam, RGB(255,0, 255) );
		return (long)(HBRUSH)CreateSolidBrush(RGB(255,0,128));//couleur de fond
		break;
	case WM_CLOSE: //byebye
		Shell_NotifyIcon(NIM_DELETE,&nf); //enleve l'icone du systray
		KillTimer(hDlg,IDM_TIMER1); //kill le timer
		PostQuitMessage(0); // renvoie 0 comme code de sortie
		return TRUE;
	}
	return FALSE;
}

VOID APIENTRY DisplayContextMenu(POINT pt) { //creation du menu cotextuel
    HMENU hmenu,hmenuTrackPopup;
	if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1))) == NULL) 
        return; 
     hmenuTrackPopup = GetSubMenu(hmenu, 0); 
	 AjouterAuMenu(hmenuTrackPopup);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_QUITTER); //charge l'image
 	SetMenuItemBitmaps(hmenuTrackPopup,IDCANCEL,MF_BYCOMMAND,bmp,bmp); //l'applique au menu choisi
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ABOUT);
	SetMenuItemBitmaps(hmenu,0xE140,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_BROWSE);
	SetMenuItemBitmaps(hmenu,IDC_BUTTON1,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_EXECUTE);
	SetMenuItemBitmaps(hmenu,0xE141,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ADD);
	SetMenuItemBitmaps(hmenu,0xE142,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_CLEAR);
	SetMenuItemBitmaps(hmenu,0xE143,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_OPEN);
	SetMenuItemBitmaps(hmenu,0xE144,MF_BYCOMMAND,bmp,bmp);
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_SAVE);
	SetMenuItemBitmaps(hmenu,0xE145,MF_BYCOMMAND,bmp,bmp);
   TrackPopupMenu(hmenuTrackPopup,TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x, pt.y, 0, hWnd, NULL); 
    DestroyMenu(hmenu); 
} 

BOOL WINAPI OnContextMenu(int x, int y) { //position pour le menu cotextuiel
    RECT rc;
	POINT pt;
    pt.x = x;
    pt.y= y;
    GetClientRect(hWnd, &rc);
    ScreenToClient(hWnd, &pt); 
    if (PtInRect(&rc, pt))     {
        ClientToScreen(hWnd, &pt); 
        DisplayContextMenu(pt); 
        return TRUE; 
    } 
    return FALSE; 
} 


void CreateMyTooltip(HWND hDlg, char* ToolTipTitre) {
	unsigned int uid = 0;//création du tooltip
	LPTSTR lptstr = (LPTSTR)ToolTipTitre;
	iccex.dwICC = ICC_WIN95_CLASSES | ICC_ANIMATE_CLASS | ICC_TAB_CLASSES;
	iccex.dwSize = sizeof(iccex);
	InitCommonControlsEx(&iccex); // initialise le contrôles
	hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP/*transforme en bulle */ | TTS_BALLOON, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, wc.hInstance, NULL); //crée la fenetre
	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	GetClientRect(hWnd, &rect);
	imghWnd = CreateWindowEx(0, "STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_ICON, 140, 1, 32, 32, hwndTT, (HMENU)45000, wc.hInstance, NULL);
	SendMessage(imghWnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON2));
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hDlg;
	ti.hinst = wc.hInstance;
	ti.uId = uid;
	ti.lpszText = lptstr;
	ti.rect.left = rect.left;
	ti.rect.top = rect.top;
	ti.rect.right = rect.right;
	ti.rect.bottom = rect.bottom;
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
	SendMessage(hwndTT, WM_USER + 20, (WPARAM)RGB(0, 255, 0), 0); //couleur du texte 
	SendMessage(hwndTT, WM_SETFONT, (WPARAM)GetStockObject(0x0E), MAKELPARAM(TRUE, 0)); //Police du tooltip
	SendMessage(hwndTT, TTM_SETTITLE, (WPARAM)(int)1, (LPARAM)(LPCTSTR)"Démo pour études");  //titre sous WINNT 5x &+ 
	SendMessage(hwndTT, WM_USER + 19, (WPARAM)RGB(204, 100, 204), 0); //fond du tooltip
	SendMessage(hwndTT, CCM_SETWINDOWTHEME, 0, (LPARAM)"BUTTON");  //theme type 
	SendMessage(hwndTT, WM_USER + 26, 0, (LPARAM)(LPRECT)&rc);  //dimension
}
void CreateMyTooltip(HWND hDlg,const char* ToolTipTitre) {
	unsigned int uid = 0;//création du tooltip
	LPTSTR lptstr = (LPTSTR)ToolTipTitre;
	iccex.dwICC = ICC_WIN95_CLASSES | ICC_ANIMATE_CLASS | ICC_TAB_CLASSES;
	iccex.dwSize = sizeof(iccex);
	InitCommonControlsEx(&iccex); // initialise le contrôles
	hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP/*transforme en bulle */ | TTS_BALLOON, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, wc.hInstance, NULL); //crée la fenetre
	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	GetClientRect(hWnd, &rect);
	imghWnd = CreateWindowEx(0, "STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_ICON, 140, 1, 32, 32, hwndTT, (HMENU)45000, wc.hInstance, NULL);
	SendMessage(imghWnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON2));
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hDlg;
	ti.hinst = wc.hInstance;
	ti.uId = uid;
	ti.lpszText = lptstr;
	ti.rect.left = rect.left;
	ti.rect.top = rect.top;
	ti.rect.right = rect.right;
	ti.rect.bottom = rect.bottom;
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
	SendMessage(hwndTT, WM_USER + 20, (WPARAM)RGB(0, 255, 0), 0); //couleur du texte 
	SendMessage(hwndTT, WM_SETFONT, (WPARAM)GetStockObject(0x0E), MAKELPARAM(TRUE, 0)); //Police du tooltip
	SendMessage(hwndTT, TTM_SETTITLE, (WPARAM)(int)1, (LPARAM)(LPCTSTR)"Démo pour études");  //titre sous WINNT 5x &+ 
	SendMessage(hwndTT, WM_USER + 19, (WPARAM)RGB(204, 100, 204), 0); //fond du tooltip
	SendMessage(hwndTT, CCM_SETWINDOWTHEME, 0, (LPARAM)"BUTTON");  //theme type 
	SendMessage(hwndTT, WM_USER + 26, 0, (LPARAM)(LPRECT)&rc);  //dimension
}

void ActiveToolTips(){// definir le texte pour chaque élément désiré
	sprintf(buffer,"ColorDLG v:1.00.B\n(C)opyright Papy67\n%s\n",appPath);
	for (int x=0; x<__argc; x++){
		strcat(buffer,__argv[x]);//ajoute chaque argument 
	}
	CreateMyTooltip (hWnd,buffer);
	CreateMyTooltip (GetDlgItem(hWnd,2),"Met fin à ce programme");
	CreateMyTooltip (GetDlgItem(hWnd,0xE140),"Affiche les informations sur ce programme");
	CreateMyTooltip (GetDlgItem(hWnd,IDOK),"Vous allez cliquer sur OK");
	CreateMyTooltip (GetDlgItem(hWnd,IDC_CHEMIN),"Ceci est une boite de saisie");

}

void AjouterAuMenu(HMENU menu){
	bmp=LoadBitmap(wc.hInstance,(LPCTSTR)IDB_ABOUT);
	AppendMenu(menu,MF_STRING,0xE140,"À propos de ce programme"); //about
	SetMenuItemBitmaps(menu,MF_BYCOMMAND,0xE140,bmp,bmp);
	AppendMenu(menu,MF_SEPARATOR,MF_STRING,"");//séparateur
	AppendMenu(menu,MF_STRING,IDC_BUTTON1,"Browse for Folders.");
	AppendMenu(menu,MF_STRING,0xE141,"ShellExecute");
	AppendMenu(menu,MF_STRING,0xE142,"AddToListe");
	AppendMenu(menu,MF_STRING,0xE143,"ClearListe");
	AppendMenu(menu,MF_STRING,0xE144,"OpenFileName");
	AppendMenu(menu,MF_STRING,0xE145,"SaveFileName");
}

OPENFILENAME CommunFichiers(LPCTSTR Titre,LPCTSTR Filtre){
	SHGetSpecialFolderPath(hWnd,szPath,CSIDL_PERSONAL,FALSE);	//obtention du dossier mes documents				
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrFilter =Filtre;
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle=Titre;
	ofn.lpstrFileTitle =NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
return ofn;
}

void OuvrirFichier(LPCTSTR Titre){
	CommunFichiers(Titre,(LPCTSTR)"Fichiers Source\0*.c;*.cpp;*.h;*.rc;*.def\0Tous les fichiers\0*.*\0\0");
	if (GetOpenFileName(&ofn)==TRUE) { //on a choisi un fichier
	HANDLE hf = CreateFile(ofn.lpstrFile, GENERIC_READ,0, (LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL); //acces au fichier
	SendMessage(GetDlgItem(hWnd,IDC_LIST1),LB_ADDSTRING, 0,(LPARAM)ofn.lpstrFile); // ajout a la liste
	SetDlgItemText(hWnd,IDC_CHEMIN,ofn.lpstrFile); //affiche le chemin du fichier
	}
}

void SauverFichier(LPCTSTR Titre){
	CommunFichiers(Titre,(LPCTSTR)"Fichiers Source\0*.c;*.cpp;*.h;*.rc;*.def\0Tous les fichiers\0*.*\0\0");
	if (GetSaveFileName(&ofn)==TRUE) { //on a choisi un fichier
	HANDLE hf = CreateFile(ofn.lpstrFile, GENERIC_WRITE,0, (LPSECURITY_ATTRIBUTES) NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,(HANDLE) NULL); //acces au fichier
	SendMessage(GetDlgItem(hWnd,IDC_LIST1),LB_ADDSTRING, 0,(LPARAM)"Sauvegarde effectuée");//ajout a la liste
	SetDlgItemText(hWnd,IDC_CHEMIN,""); //affiche le chemin du fichier
	}

}
void ExplorerDossiers(LPCTSTR Titre){
    bi.hwndOwner=hWnd;
	bi.pidlRoot=NULL;
	bi.pszDisplayName=&buffer[0];
	bi.lpszTitle=Titre;
	bi.ulFlags=BIF_RETURNONLYFSDIRS |BIF_DONTGOBELOWDOMAIN|BIF_STATUSTEXT |BIF_EDITBOX ;
	bi.lpfn=NULL; 
	il=SHBrowseForFolder(&bi);
	if (il==NULL) return ;
	if(SHGetPathFromIDList(il,&Result[0])){
		SendMessage(GetDlgItem(hWnd,IDC_LIST1),LB_RESETCONTENT, 0,0);
		SetDlgItemText(hWnd,IDC_CHEMIN,Result);
	}
}

