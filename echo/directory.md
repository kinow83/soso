### directory 구조
---
### 1. [repository]
```
        |--------- [src]     # 소스코드 디렉토리
        |--------- [include] # 헤더 디렉토리
        |--------- [docs]    # doxygen 결과물
        |--------- [build]   # build 디렉토리는 .gitignore에 포함. CMake build 위치
        |
        |--------- CMakeLists.txt # CMake 파일
        |--------- Doxyfile       # doxygen 설정 파일
        |--------- LICENSE
        |--------- configure.sh   # 빌드 스크립트        
```
        
        
### 2. configure.sh
```
cmake -B build -S .
cmake --build build
```

### 3. .gitignore
```
build
```
