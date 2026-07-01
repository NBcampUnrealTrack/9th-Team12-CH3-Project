# Pull Request

## 1. 변경 요약
<!-- 이번 PR에서 변경한 내용을 2~5줄로 요약해주세요. -->
-
-
-

## 2. 작업 유형
<!-- 해당하는 항목에 x 표시해주세요. -->
- [ ] feat: 신규 기능 추가
- [ ] fix: 버그 수정
- [ ] refactor: 구조 개선 / 리팩토링
- [ ] chore: 빌드 설정 / 플러그인 / 의존성 / 환경 설정 변경
- [ ] docs: 문서 수정
- [ ] ci: GitHub Actions / 자동화 설정 변경
- [ ] hotfix: 긴급 수정

## 3. 관련 이슈 / 작업 링크
<!-- 예: Closes #12, Related #34, Notion 작업 링크 등 -->
-

## 4. 변경 범위
<!-- 영향받은 영역을 체크해주세요. -->
- [ ] C++ 코드
- [ ] Blueprint
- [ ] Animation / Montage / AnimBP
- [ ] UI / Widget
- [ ] Input / Enhanced Input
- [ ] AI / Behavior Tree / EQS / Navigation
- [ ] Character / Movement
- [ ] Enemy / Spawner
- [ ] Weapon / Projectile / Damage
- [ ] DataAsset / DataTable
- [ ] Map / Level / World Partition
- [ ] Material / Niagara / VFX / SFX
- [ ] Multiplayer / Replication / Session
- [ ] Save / Load
- [ ] Plugin / Build.cs / Target.cs
- [ ] Git LFS / .gitattributes / .gitignore
- [ ] 기타:

## 5. 구현 내용
<!-- 핵심 구현 내용을 구체적으로 적어주세요. 리뷰어가 코드 보기 전 흐름을 이해할 수 있어야 합니다. -->
-
-
-

## 6. 테스트 결과
<!-- 직접 확인한 테스트만 체크해주세요. 확인하지 않은 항목은 비워두세요. -->
- [ ] Unreal Editor 실행 확인
- [ ] C++ 빌드 성공
- [ ] Blueprint Compile 오류 없음
- [ ] PIE 실행 확인
- [ ] Standalone 실행 확인
- [ ] 주요 맵 로딩 확인
- [ ] 신규/수정 기능 정상 동작 확인
- [ ] 기존 기능 회귀 테스트 확인
- [ ] 멀티플레이 기능인 경우 Listen Server / Client 확인
- [ ] 패키징 확인
- [ ] 테스트 미진행

### 테스트 환경
- Unreal Engine 버전:
- IDE: Rider / Visual Studio / 기타
- OS:
- 테스트 맵:
- 테스트 인원 수: 1인 / 2인 이상 / 해당 없음

### 테스트 방법
<!-- 어떤 순서로 테스트했는지 적어주세요. -->
1.
2.
3.

## 7. 스크린샷 / 영상
<!-- UI, VFX, 애니메이션, 레벨, 플레이 흐름 변경이 있으면 첨부해주세요. -->
-

## 8. 에셋 / LFS 변경 사항
<!-- Unreal 프로젝트는 .uasset, .umap 같은 바이너리 파일 충돌 위험이 큽니다. 반드시 작성해주세요. -->
- [ ] .uasset 변경 있음
- [ ] .umap 변경 있음
- [ ] 신규 에셋 추가 있음
- [ ] 에셋 삭제 있음
- [ ] Git LFS 추적 대상 파일 추가/수정 있음
- [ ] 해당 없음

### 변경된 주요 에셋
<!-- 예: Content/Characters/Enemy/BP_Enemy.uasset -->
-
-

### 주의 필요한 에셋 충돌 가능성
<!-- 같은 맵/블루프린트를 다른 팀원이 수정 중이면 반드시 적어주세요. -->
-

## 9. 리뷰어 확인 포인트
<!-- 리뷰어가 집중해서 봐야 할 부분을 적어주세요. -->
-
-
-

## 10. 리스크 / 미해결 사항
<!-- 임시 코드, 성능 우려, 추후 작업, 버그 가능성 등을 적어주세요. -->
- [ ] 임시 코드 있음
- [ ] 성능 이슈 가능성 있음
- [ ] 메모리 / GC / 참조 관리 확인 필요
- [ ] 네트워크 동기화 확인 필요
- [ ] 에셋 충돌 가능성 있음
- [ ] 추후 작업 필요
- [ ] 해당 없음

### 상세 내용
-

## 11. 병합 전 체크리스트
<!-- PR 작성자가 직접 확인해주세요. -->
- [ ] 작업 브랜치가 최신 dev 기준인지 확인했습니다.
- [ ] 불필요한 파일이 포함되지 않았습니다. 예: Binaries, Intermediate, Saved, DerivedDataCache
- [ ] 의도하지 않은 .uasset / .umap 변경이 없는지 확인했습니다.
- [ ] Git LFS 대상 파일이 정상적으로 추적되는지 확인했습니다.
- [ ] 로그성 코드, 테스트용 액터, 디버그 출력이 남아있지 않은지 확인했습니다.
- [ ] Build.cs / Target.cs / 플러그인 변경 시 팀원이 추가 설정해야 할 내용을 작성했습니다.
- [ ] 충돌 가능성이 큰 맵/블루프린트 변경 사항을 설명했습니다.
- [ ] 리뷰어가 재현 가능한 테스트 방법을 작성했습니다.

## 12. 추가 설명
<!-- 리뷰어에게 남길 메모가 있으면 작성해주세요. -->
-

---

<!--
작성 기준:
- PR은 “무엇을 바꿨는지”보다 “왜 바꿨고, 어디를 보면 되는지”가 더 중요합니다.
- Unreal 프로젝트에서는 바이너리 에셋 충돌이 치명적이므로 .uasset / .umap 변경 여부를 명확히 남겨주세요.
- 테스트하지 않은 항목은 체크하지 마세요. 체크리스트는 책임 범위 표시입니다.
-->
