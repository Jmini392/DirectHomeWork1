#include "Time.h"

CTime::CTime() {
	FrameRate = 0;
	FpsCheckTime = 0.0f;
	PrevTime = std::chrono::high_resolution_clock::now();
}

void CTime::Tick(float TargetFrameRate) {
	// 프레임 고정 - 목표 시간이 될 때까지 대기
	//if (TargetFrameRate > 0.0f) {
	//	float targetDelay = 1.0f / TargetFrameRate;
	//	while (true) {
	//		CurrentTime = std::chrono::high_resolution_clock::now();
	//		std::chrono::duration<float> elapsed = CurrentTime - PrevTime;
	//		if (elapsed.count() >= targetDelay) {
	//			break; // 목표한 대기 시간이 지났으므로 루프 탈출
	//		}
	//	}
	//}
	//else
	CurrentTime = std::chrono::high_resolution_clock::now();

	// 실제 경과 시간 도출 및 이전 시간 갱신
	std::chrono::duration<float> deltaTime = CurrentTime - PrevTime;
	PrevTime = CurrentTime;

	// 1초 단위 누적 FPS 계산
	static unsigned long frameCount = 0;
	frameCount++;
	FpsCheckTime += deltaTime.count();

	if (FpsCheckTime >= 1.0f) {
		FrameRate = frameCount;  // 1초간 누적된 횟수를 최종 FPS로 확정
		frameCount = 0;          // 카운트 리셋
		FpsCheckTime -= 1.0f;    // 오차 누적을 막기 위해 1.0씩 감산
	}
}

unsigned long CTime::PrintFrameRate(LPTSTR lpszString, int nCharacters) {
	if (lpszString) {
		_itow_s(FrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}

	return(FrameRate);
}