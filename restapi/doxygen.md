# doxygen 사용 규칙

### 1. doxygen 설정 파일 생성
---
```
]# doxygen -g
Doxyfile 파일 생성됨
```

### 2. doxygen 설정 파일 기본 설정
---
```
PROJECT_NAME = "My Project"
OUTPUT_LANGUAGE = Korean
EXTRACT_PRIVATE = YES  // private 멤버도 출력
INPUT = include src // 소스 위치 경로
```

### 3. doxygen 설정 파일 기본 설정
---
```
]# doxygen Doxyfile
html latex // 생성된 디렉토리
html/index.html // doxygen 웹 페이지
```

### 4. doxygen 사용 팁
---
```
1. 멤버 변수 문서화
/// : 멤버 변수 바로 위에 기술
///< : 멤버 변수 바로 옆에 기술
```

### 5. github에 doxygen 웹 페이지 연동
---
1. git repository > Setting으로 이동 후 Pages 설정
![스크린샷, 2021-06-07 14-26-12](https://user-images.githubusercontent.com/26860897/120963357-64820400-c79c-11eb-8e5f-2b0734422752.png)
2. Branch를 main으로 선택 후 /docs를 선택, docs 디렉토리가 있어야 하며 docs에는 doxygen 출력을 저장한다.
![스크린샷, 2021-06-07 14-28-38](https://user-images.githubusercontent.com/26860897/120963499-b034ad80-c79c-11eb-8f77-24b9bf25c3ac.png)
3. repository 메인 readme.md에 doxygen index.html 링크 표시
```
예시> 

# ModuleWorker
---
[doxygen](https://kinow83.github.io/ModuleWorker/html/annotated.html)
---
```
![스크린샷, 2021-06-07 14-32-14](https://user-images.githubusercontent.com/26860897/120963786-2b965f00-c79d-11eb-918c-37638d076e21.png)

### 6. 라이선스
recommand MIT
