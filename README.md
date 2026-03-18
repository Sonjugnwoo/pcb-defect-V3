PCB 불량 검출 시스템 v3.0  
OpenCV + MFC + YOLOv3 기반 실시간 결함 탐지  

기능 개요  
이미지 로드: 더 단위 배치 로딩 + 리스트박스 더블클릭  
YOLOv3 검출: DeepPCB 훈련 모델 + 바운딩박스 실시간 시각화  
배치 처리: 전체 폴더 순차 자동 처리 + 진행률 표시  
결과 저장: 불량 이미지(Defect_XXXX.jpg) + 통계 리포트(UTF-8)  
UI: Picture Control 실시간 갱신 + 상태바 클래스별 카운트    

기술 스택  
Frontend: MFC Dialog + Picture Control  
Backend: OpenCV 4.x + YOLOv3 (darknet)  
딥러닝: YOLOv3 (yolov3-pcb.weights + pcb-error.names)  
빌드: Visual Studio 2019 + vcpkg  
GPU: CUDA 지원 (RTX 2060s)  

배치 처리 파이프라인  
1. 폴더 로드 → 리스트박스 표시  
2. [일괄 실행] → for(i=0; i<nCount; i++)  
3. imgLoad() → DetectorPcbDefect() → DrawMat()  
4. ClassCount() 실시간 상태 갱신  
5. Report() → PCB_Report.txt + Defect_XXXX.jpg 저장

성능 지표  
- YOLOv3 mAP: 95%+ (DeepPCB 데이터셋 기준)  
- 배치 처리: 46.7ms/이미지   (300장/14초)
- 클래스 수: 6종 (short, missing_hole, mouse_bite 등)

한계점 & 개선점  
kaggle dateset 추가 학습  
불량만 필터링  
불량 이미지 저장 , 보고서 작성  


개발 로드맵
v1.0 - 템플릿 매칭 프로토타입 ✅  
v2.0 - YOLOv3 배치 처리 ✅  
v3.0 - 불량만 추출 ✅    
v4.0 - 실시간 카메라 스트림    

![Image](https://github.com/user-attachments/assets/384d2d0d-6640-404a-b8fe-1d6ed9f8a096)
![Image](https://github.com/user-attachments/assets/96fc30ed-9110-48b5-9037-c6be3b1ae495)
![Image](https://github.com/user-attachments/assets/58a60592-f35b-41c9-8a25-901a66ba751d)
