
// PCBdefectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "PCBdefect.h"
#include "PCBdefectDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPCBdefectDlg 대화 상자


CPCBdefectDlg::CPCBdefectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCBDEFECT_DIALOG, pParent)
	, m_selectedFolder(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPCBdefectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATH, m_selectedFolder);
	DDX_Control(pDX, IDC_LIST_BOX, curDir);
	DDX_Control(pDX, IDC_PIC, m_pic_con);
}

BEGIN_MESSAGE_MAP(CPCBdefectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &CPCBdefectDlg::OnBnClickedOpen)
	ON_LBN_DBLCLK(IDC_LIST_BOX, &CPCBdefectDlg::OnLbnDblclkListBox)
	ON_BN_CLICKED(IDC_EXPAND, &CPCBdefectDlg::OnBnClickedExpand)
	ON_BN_CLICKED(IDC_BATCH, &CPCBdefectDlg::OnBnClickedBatch)
	ON_BN_CLICKED(IDC_CLEAR, &CPCBdefectDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_CLOSE, &CPCBdefectDlg::OnBnClickedClose)
END_MESSAGE_MAP()


// CPCBdefectDlg 메시지 처리기

BOOL CPCBdefectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
		// 실행파일(exe)이 저장된 디렉토리 경로 얻기...
	// -----------------------------------------
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH); // "...\Exe\ObjectDetector.exe" 경로
	CString msg(path);
	int n = msg.ReverseFind('\\'); // ...\Exe\ 까지 위치, 0,1,...n
	m_strHome = msg.Left(n + 1); // ...\Exe\ 까지 문자열 복사, 좌측에서 (n+1)개 복사
	int n_out = msg.Left(n).ReverseFind('\\');
	Out = msg.Left(n_out+1) + L"Out";
	// -----------------------------------------
	// Initialize Darknet YOLOv3 detector & load model files
	// -----------------------------------------
	CString str_cfg = m_strHome + L"yolov3-pcb.cfg";
	CString str_wgt = m_strHome + L"yolov3-pcb.weights";
	std::string cfg = std::string(CT2CA(str_cfg.operator LPCWSTR()));
	std::string wgt = std::string(CT2CA(str_wgt.operator LPCWSTR()));
	m_pDetector = new Detector(cfg, wgt);

	m_vecClassName.clear();
	wchar_t buffer[125];
	// coco.names -> UTF-8 decoding 
	CString namePath = m_strHome + L"pcb-error.names";
	FILE* stream = _wfopen(namePath, L"rt+,ccs=UTF-8");
	if (stream == NULL) {
		AfxMessageBox(L"Can't open " + namePath);
		return false;
	}
	// 텍스트를 한 줄씩 읽어서 vector에 저장한다.
	// 한 줄 읽으면 끝에 dummy 문자가 추가된 것 제거
	while (fgetws(buffer, 125, stream) != NULL) {
		CString strClass(buffer);
		CString strOnlyChar = strClass.Left(strClass.GetLength() - 1);
		m_vecClassName.push_back(strOnlyChar);
	}
	fclose(stream);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
// 
// ======================================================================
// 폴더 선택 대화상자의 콜백 함수
// BFFM_INITIALIZED 메시지에서 초기 폴더 경로를 설정
// ======================================================================
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	// 콜백 메시지별 처리
	switch (uMsg) {
	case BFFM_INITIALIZED:              // 대화상자가 초기화될 때 호출됨
		// lpData에 전달된 초기 경로(C:\)를 대화상자에 선택 상태로 설정
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;                           // 콜백 처리 완료
}
void CPCBdefectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPCBdefectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCBdefectDlg::OnBnClickedOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ITEMIDLIST* pidlBrowes;         // 폴더 선택 결과를 담는 구조체 포인터
	WCHAR pszPathname[MAX_PATH];    // 선택된 폴더의 전체 경로를 저장할 배열

	BROWSEINFO BrInfo;              // 폴더 선택 대화상자 설정에 사용하는 구조체

	BrInfo.hwndOwner = GetSafeHwnd();   // 부모 윈도우 핸들 등록 (현재 다이얼로그)
	BrInfo.pidlRoot = NULL;             // 루트 폴더를 지정하지 않음 (기본은 "내 컴퓨터")

	memset(&BrInfo, 0x00, sizeof(BrInfo));  // 구조체를 0으로 초기화
	BrInfo.pszDisplayName = pszPathname;    // 선택된 폴더명이 저장될 버퍼
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;  // 실제 파일 시스템 폴더만 선택 가능 (가상 폴더 제외)

	BrInfo.lpfn = BrowseCallbackProc;       // 콜백 함수 포인터 (초기 위치 설정 등에 사용 가능)

	CString strInitPath = _T("C:\\");       // 폴더 대화상자 초기 경로 설정
	BrInfo.lParam = (LPARAM)strInitPath.GetBuffer();  // 초기 경로를 콜백에 전달

	// 폴더 선택 대화상자 실행
	pidlBrowes = ::SHBrowseForFolder(&BrInfo);

	// 사용자가 폴더를 선택했을 경우
	if (pidlBrowes != NULL) {
		SHGetPathFromIDList(pidlBrowes, pszPathname);  // ITEMIDLIST → 실제 경로 문자열로 변환

		CString folderPath(pszPathname);   // 예: "C:\\PCB_images"
		CString tpath;

		SetDlgItemText(IDC_PATH, folderPath);

		tpath.Format(_T("%s\\*.jpg"), folderPath);  // 선택 폴더 내의 모든 JPG 파일 검색 경로

		m_selectedFolder = folderPath;     // 멤버 변수에 선택된 폴더 경로 저장
		CFileFind finder;                  // 파일 탐색용 CFileFind 객체
		BOOL bWorking = finder.FindFile(tpath);  // 첫 파일 검색 시작
		curDir.ResetContent();            // 리스트박스 내용 초기화
		CString fileName, DirName;

		// 폴더 내 모든 jpg 파일 순회
		while (bWorking) {
			bWorking = finder.FindNextFile();  // 다음 파일 검색

			if (finder.IsDirectory() || finder.IsDots())	// 폴더나 . .. 는 건너뛰기
				continue;

			fileName = finder.GetFileTitle();        // 확장자 제외한 파일명만 추출

			curDir.AddString(fileName);             // 리스트박스에 파일명 추가
		}
	}
}


void CPCBdefectDlg::OnLbnDblclkListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString fileName;           // 선택된 파일명을 저장할 변수
	idx = curDir.GetCurSel();   // 현재 리스트 박스에서 선택된 항목의 인덱스를 가져옴

	curDir.GetText(idx, fileName);  // 인덱스에 해당하는 항목의 문자열(파일명)을 fileName에 저장

	cv::Mat img = imgLoad(fileName); // 이미지 파일을 OpenCV Mat 형태로 로드 
	DrawMat(img);                    // Picture Control에 이미지를 표시 
}


void CPCBdefectDlg::OnBnClickedExpand()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (dClickImg.empty()) {
		SetDlgItemText(IDC_STATUS, _T("선택된 이미지가 없습니다. \n"));
		return;  // 조기 종료
	}
	// 확대용 새 cv::Mat 생성 
	cv::Mat zoomed;

	// 1.5배 확대 리사이즈 (가로/세로 동시 확대)
	// cv::INTER_CUBIC: 부드러운 확대
	cv::resize(dClickImg, zoomed,
		cv::Size(dClickImg.cols * 1.5, dClickImg.rows * 1.5),  // 출력 크기
		0, 0,                                                    // x/y 축 스케일 팩터 (0=자동)
		cv::INTER_CUBIC);                                        // 보간 방법

	// 메인 윈도우를 데스크톱 중앙으로 이동 (확대창 배치 준비)
	CenterWindow(GetDesktopWindow());

	// OpenCV 고정 크기 윈도우 생성 (크기 자동 조정)
	cv::namedWindow("확대 이미지", cv::WINDOW_AUTOSIZE);

	// 확대된 바운딩박스 이미지 표시
	cv::imshow("확대 이미지", zoomed);

	// 키보드 입력 대기 (0=무한대기, ESC키 입력까지 블로킹)
	int key = cv::waitKey(0);

	// 모든 OpenCV 윈도우 파괴 (메모리 정리)
	cv::destroyAllWindows();

	// ESC키(27) 입력 확인 및 상태 메시지 업데이트
	if (key == 27) {
		dClickImg.release();
		SetDlgItemText(IDC_STATUS, _T("확대 이미지를 닫았습니다."));
	}
}


void CPCBdefectDlg::OnBnClickedBatch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// ================================
	// 리스트박스 파일 확인
	// ================================
	if (curDir.GetCount() == 0) {	    // 리스트박스가 비어있는지 확인
		SetDlgItemText(IDC_STATUS, L"폴더를 먼저 선택해 주세요\n");
		return;
	}
	int nCount = curDir.GetCount();                     // 처리할 총 파일 개수
	DWORD start = GetTickCount();                       // 배치 처리 시작 시간 측정
	SetDlgItemText(IDC_STATUS, L"배치 처리 중...");     // 진행 상태 표시
	// ================================
	// 모든 파일 순차 처리
	// ================================
	for (int i = 0; i < nCount; i++) {
		CString fileName;
		curDir.GetText(i, fileName);					// i번째 파일명 가져오기

		cv::Mat img = imgLoad(fileName);				// 이미지 로드
		DetectorPcbDefect(img, i);                      // YOLO 결함 탐지 실행 (결함 결과를 pcbDefect에 저장)
		DrawMat(img);                                   // 탐지 결과 이미지 화면에 표시
		CString status = ClassCount();                  // 현재까지의 클래스별 카운트 문자열 생성
		SetDlgItemText(IDC_STATUS, status);             // 실시간 상태 업데이트
	}
	// ================================
	// 처리 시간 계산
	// ================================
	DWORD end = GetTickCount();                         // 배치 처리 종료 시간
	DWORD elapsed = end - start;                        // 경과 시간(ms)
	int seconds = elapsed / 1000;                       // 초 단위
	int ms = elapsed % 1000;                            // 밀리초 단위
	// ================================
	// 저장 여부 사용자 확인
	// ================================
	int nMessage = AfxMessageBox(_T("파일을 저장 하시겠습니까\n"), MB_OKCANCEL, MB_ICONSTOP);
	if (nMessage == IDOK) {
		for (int i = 0; i < pcbDefect.size(); i++) {
			CString pcbDefectName;
			pcbDefectName.Format(L"%s\\Defect_%04d.jpg", Out, pcbDefect[i].second);
			std::string path = std::string(CT2CA(pcbDefectName.operator LPCWSTR()));

			cv::imwrite(path, pcbDefect[i].first);		 // 결함 이미지를 JPG로 저장
		}
		// ================================
		// 리포트 텍스트 파일 생성 (UTF-8 BOM 포함)
		// ================================
		CString save = Report();							// 전체 리포트 문자열 생성
		CString filename = Out + _T("\\PCB_Report.txt");	// 리포트 파일명
		CFile file;
		if (file.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
			// UTF-8 BOM 추가 (한글 깨짐 방지)
			BYTE utf8bom[] = { 0xEF, 0xBB, 0xBF };
			file.Write(utf8bom, 3);

			// CString → UTF-8 변환
			int utf8size = WideCharToMultiByte(CP_UTF8, 0, save, -1, NULL, 0, NULL, NULL);
			char* utf8buf = new char[utf8size];
			WideCharToMultiByte(CP_UTF8, 0, save, -1, utf8buf, utf8size, NULL, NULL);

			file.Write(utf8buf, utf8size - 1);  // NULL 제외
			file.Close();
			delete[] utf8buf;
		}

	}
	else {
		pcbDefect.clear();	 // 저장 안할 경우 메모리 정리
	}
	// ================================
	//완료 메시지 표시
	// ================================
	CString status;
	status.Format(_T("총 %d개 완료\r\n걸린 시간: %d.%03d초"), nCount, seconds, ms);
	SetDlgItemText(IDC_STATUS, status);	 // 최종 결과 표시
}


void CPCBdefectDlg::OnBnClickedClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	boundingImg.release();              // OpenCV 이미지 메모리 해제 (중요!)
	idx = 0;                            // 현재 인덱스 초기화

	// Picture Control을 회색 배경으로 초기화
	CRect rt;
	m_pic_con.GetClientRect(&rt);       // Picture Control 클라이언트 영역 좌표 가져오기
	CDC* dc = m_pic_con.GetDC();        // Device Context 획득
	dc->FillSolidRect(&rt, RGB(240, 240, 240)); // 연한 회색(RGB 240,240,240)으로 채우기
	m_pic_con.ReleaseDC(dc);            // DC 반납 (메모리 누수 방지)

	// Picture Control 갱신으로 화면 즉시 반영
	m_pic_con.Invalidate();             // 무효화하여 다시 그리기 요청
	m_pic_con.UpdateWindow();           // 즉시 업데이트

	SetDlgItemText(IDC_STATUS, _T("초기화 완료\n")); // 상태창에 완료 메시지 표시

}


void CPCBdefectDlg::OnBnClickedClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	boundingImg.release();              // OpenCV 이미지 메모리 해제
	delete m_pDetector;                 // YOLO Detector 객체 삭제 (동적 할당 해제)

	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}
