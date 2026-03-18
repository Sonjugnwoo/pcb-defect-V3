#include "pch.h"
#include "PCBdefectDlg.h"

// 클래스별 고정 색상 정의
static cv::Scalar classColors[] = {
    cv::Scalar(0, 255, 0),      // 0: Green  - open_circuit 
    cv::Scalar(0, 0, 255),      // 1: Red    - short
    cv::Scalar(255, 0, 0),      // 2: Blue   - mouse_bite
    cv::Scalar(0, 255, 255),    // 3: Yellow - spurious_copper 
    cv::Scalar(255, 0, 255),    // 4: Magenta- spur
    cv::Scalar(255, 255, 0)     // 5: Cyan   - pin_hole 

};  
// =====================================================
// 이미지 컨트롤에 이미지를 그리기 위한 공통 함수
// img: 표시할 이미지 객체
// control: 그림을 그릴 Picture Control
// =====================================================
void CPCBdefectDlg::DrawMat(const cv::Mat& mat) {
    // Picture Control의 클라이언트 영역 크기 가져오기
    CRect rect;
    m_pic_con.GetClientRect(&rect);
    

    // DC(Device Context) 획득 (그리기용)
    CDC* dc = m_pic_con.GetDC();

    //  BITMAPINFO 구조체 초기화 (Windows DIB 포맷)
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = mat.cols;                    // 원본 너비
    bmi.bmiHeader.biHeight = -mat.rows;                  // 높이(음수=top-down)
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // 채널 수에 따른 비트 깊이 설정
    if (mat.channels() == 1) {
        bmi.bmiHeader.biBitCount = 8;     // 그레이스케일
    }
    else {
        bmi.bmiHeader.biBitCount = 24;    // 컬러 BGR
        bmi.bmiHeader.biCompression = BI_RGB;
    }

    //    StretchDIBits()로 이미지 렌더링
    //    - 목적지: (0,0) ~ (rect.Width(), rect.Height())
    //    - 원본: mat 전체 영역
    //    - DIB_RGB_COLORS: 팔레트 색상 매핑
    //    - SRCCOPY: 픽셀 복사
    ::StretchDIBits(dc->GetSafeHdc(),
        0, 0, rect.Width(), rect.Height(),      // 목적지: 컨트롤 전체 크기
        0, 0, mat.cols, mat.rows,               // 원본: 이미지 전체
        mat.data, &bmi, DIB_RGB_COLORS, SRCCOPY);
    // DC 해제 (메모리 누수 방지)
    m_pic_con.ReleaseDC(dc);
}
// =====================================================
// 리스트박스에서 선택한 이미지 파일을 로드하여 화면에 표시
// fileName: 리스트박스에 표시된 파일명 (확장자 제외, 예: "template001")
// =====================================================
cv::Mat CPCBdefectDlg::imgLoad(const CString fileName) {
    CString fullPath;                   // 완전한 파일 경로를 저장할 변수
    // 선택된 폴더 + 파일명 + .jpg 확장자로 전체 경로 생성
    fullPath.Format(_T("%s\\%s.jpg"), m_selectedFolder, fileName);

    // CString → std::string 변환
    std::string filePath;
    CT2A converter(fullPath);  // CString → char*
    filePath = converter;      // char* → std::string

    cv::Mat mat = cv::imread(filePath);

    if (mat.empty()) {
        AfxMessageBox(_T("로드 실패! 파일 확인하세요"));
        return cv::Mat();
    }
    dClickImg = mat;
    return mat;
}

void CPCBdefectDlg::DetectorPcbDefect( cv::Mat & img,int index ) {
    // ===== 입력 검증 =====
    if (img.empty()) {
        AfxMessageBox(_T("로드 실패! 파일 확인하세요"));
        return;
    }
    cv::Mat frame = img.clone();                    // 원본 이미지 백업 (YOLO 입력용)
    std::vector<bbox_t> defected = m_pDetector->detect(frame, threshold);    // YOLO 탐지 실행 (threshold=0.7)
    boundingImg.release();                          // 이전 결과 이미지 메모리 해제

    // ===== 각 결함에 대해 바운딩박스 그리기 =====
    for (int i = 0; i < defected.size(); i++) {
        CString name = m_vecClassName[defected[i].obj_id];              // 클래스명 조회
        std::string text = std::string(CT2CA(name.operator LPCWSTR())); // CString → std::string

        cv::Scalar color = classColors[(defected[i].obj_id) % 6];       // 클래스ID에 따른 색상 선택 (6가지 색상 반복)

        // ===== 통계 집계 =====
        m_classCount[defected[i].obj_id]++;         // 실시간 상태 표시용 카운트 증가
        m_reportCount[defected[i].obj_id]++;        // 최종 리포트용 카운트 증가

        // ===== 바운딩박스 및 텍스트 그리기 =====
        cv::Point left(defected[i].x, defected[i].y);                   // 좌상단 좌표
        cv::Point right(defected[i].x + defected[i].w, defected[i].y + defected[i].h); // 우하단 좌표
        cv::Point textPoint(defected[i].x, defected[i].y - 10);         // 텍스트 위치 (박스 위)

        cv::rectangle(img, left, right, color, 3);      // 바운딩박스 그리기 (두께 3)
        cv::putText(img, text, textPoint, cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
        // 클래스명 텍스트 그리기 (크기 0.6, 두께 2)
    }
    if(!defected.empty())
        boundingImg = img.clone();                      // 최종 결과 이미지 저장
    //===== 불량 이미지 저장 여부 판단 =====
    if (!boundingImg.empty())
        pcbDefect.push_back(std::make_pair(boundingImg, index));      // 결함이 탐지된 경우 pcbDefect 벡터에 추가

}

CString CPCBdefectDlg::ClassCount() {
    CString Text;
    int textlen = 0;                        // 한 줄에 표시할 클래스 개수 카운터 (최대 3개)
    int totalDefect = 0;                    // 발견된 총 결함 개수
    // ===== 총 결함 개수 계산 =====
    for (const auto& product : m_classCount) {
        totalDefect += product.second;        // 모든 클래스 결함 개수 합계
    }

    // ===== 클래스별 결함 목록 생성 (3개씩 줄바꿈) =====
    Text.Format(_T("발견된 결함 : %d\n"), totalDefect);
    for (const auto& product : m_classCount) {
        textlen++;                          // 한 줄 클래스 카운터 증가
        int classId = product.first;        // 클래스 ID
        int count = product.second;         // 결함 개수
        CString className = m_vecClassName[classId]; // 클래스명 조회

        Text.AppendFormat(_T("  - %s: %d개   "), className, count);

        if (textlen == 3) {                 // 3개 클래스 출력 후 줄바꿈
            Text.AppendFormat(_T("\n"));
            textlen = 0;                    // 카운터 초기화 (다음 줄 시작) 
        }
    }
    // ===== 다음 이미지 처리를 위한 초기화 =====
    m_classCount.clear();                   // 현재 이미지의 클래스 카운트 초기화
    if (totalDefect == 0)
        Text = _T("이상 없음\n");          // "이상 없음"으로 상태창 변경 
    else
        report.push_back(Text);             // 결함이 있을 경우 report 벡터에 저장 (배치 리포트용)

    return Text;                            // 상태창에 표시할 최종 문자열 반환

}
CString CPCBdefectDlg::Report() {
    CString Text;
    int totalDefect = 0;                    // 발견된 총 결함 개수
    int totalidx = curDir.GetCount();      // 총 처리한 PCB 개수
    int totalPoor = pcbDefect.size();   // 불량 PCB 개수
    int totalPass = totalidx - totalPoor;  // 정상 PCB 개수
    // ===== 제목 및 전체 통계 =====
    Text.Format(_T("===== 배치처리 결과 ===== \r\n"));
    Text.AppendFormat(_T("-총 : %d , 정상 : %d , 불량 %d\r\n"),totalidx, totalPass, totalPoor);
    Text.AppendFormat(_T("불량 률 %.2f%% \r\n"), (double)totalPoor / totalidx * 100);   // 불량률 계산
    for (const auto& product : m_reportCount)   // m_reportCount 맵 순회
        totalDefect += product.second;          // 모든 클래스 결함 개수 합계
    // =====  클래스별 결함 통계 =====
    Text.AppendFormat(_T("발견된 총 결함 : %d\r\n"), totalDefect); 
    for (const auto& product : m_reportCount) {
        int classId = product.first;            // 클래스 ID
        int count = product.second;             // 해당 클래스 결함 개수
        CString className = m_vecClassName[classId]; // 클래스명 조회
        Text.AppendFormat(_T("  - %s: %d개\r\n"), className, count);

    }
    // =====  개별 PCB 상세 결과 ==== =
    int i = 0; 
    for (const auto& product : pcbDefect) {
        Text.AppendFormat(_T("===== %d_PCB ===== \r\n"), product.second); // PCB 번호(클래스ID)
        Text += report[i++];                                            // 해당 PCB의 상세 리포트 추가
        Text.AppendFormat(_T("\r\n"));                                   // 구분선
    }

    return Text;
}