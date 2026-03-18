
// PCBdefectDlg.h: 헤더 파일
//

#pragma once


// CPCBdefectDlg 대화 상자
class CPCBdefectDlg : public CDialogEx
{
private:
	CString m_strHome; // 프로그램 실행 경로 또는 root 디렉터리
	cv::Mat boundingImg; // 탐지된 박스가 그려진 이미지
	std::map<int, int> m_classCount; // 클래스별 검출 개수 저장 (key: 클래스 ID, value: 개수)
	std::map<int, int> m_reportCount; // 리포트용 클래스별 카운트
	std::vector<CString> report; // 결과 리포트 문자열 목록
	std::vector<std::pair<cv::Mat, int>> pcbDefect; // (결함 이미지, 클래스 ID) 쌍 저장
	int idx; // 현재 선택된 이미지 또는 탐지 인덱스
	cv::Mat dClickImg; // 더블클릭으로 선택된 이미지 저장
	double threshold = 0.7; // 탐지 임계값 (검출 신뢰도 임계치)
// 생성입니다.
public:
	CPCBdefectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CString Out; // 배치 파일 및 로그 파일 저장 경로
	Detector* m_pDetector; // YOLO Object Detector 포인터
	std::vector<CString> m_vecClassName; // 클래스 이름 목록 (YOLO class names)
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PCBDEFECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	// ------------------------------------------------------------------
	// 내부 처리 함수
	// ------------------------------------------------------------------
	void DrawMat(const cv::Mat& img);                  // OpenCV 이미지를 MFC 컨트롤에 표시
	cv::Mat imgLoad(const CString fileName);           // 파일에서 이미지 로드 후 반환
	void DetectorPcbDefect(cv::Mat& img, int index);   // YOLO를 이용해 PCB 결함 탐지 수행
	CString ClassCount();                              // 클래스별 탐지 개수 문자열 반환
	CString Report();                                  // 리포트 생성 문자열 반환
// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	CString m_selectedFolder; // 선택된 이미지 폴더 경로
	CListBox curDir; // 폴더 내 파일 리스트를 표시하는 리스트박스
	CStatic m_pic_con; // 탐지 이미지 표시용 Picture Control

	afx_msg void OnLbnDblclkListBox(); // 리스트 더블클릭 시 이미지 표시 및 탐지 실행
	afx_msg void OnBnClickedOpen(); // [열기] 버튼 클릭 시 폴더 선택 및 이미지 로드
	afx_msg void OnBnClickedExpand(); // [확대] 버튼 클릭 시 이미지 확대 창 표시
	afx_msg void OnBnClickedBatch(); // [일괄 실행] 버튼 클릭 시 폴더 내 모든 이미지 탐지
	afx_msg void OnBnClickedClear(); // [초기화] 버튼 클릭 시 데이터 초기화
	afx_msg void OnBnClickedClose(); // [종료] 버튼 클릭 시 다이얼로그 종료
};
