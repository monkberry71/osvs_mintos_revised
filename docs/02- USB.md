## USB
물리적으로는 플로피와 다르지만 논리적으로는 어찌됐던 둘 다 디스크 드라이브임. 둘 다 블록 단위 IO와 CHS(Cylinder, Head, Sector) 어드레스 방식 또는 LBA(Logical Block Adress)를 지원함.

USB도 플로피처럼 CHS 방식으로 접근할 수 있기 때문에 int 0x13으로 플로피 디스크마냥 읽고 쓸 수 있음. 하지만, 드라이브 번호를 지정하고 섹터->헤드->트랙 순으로 값을 증가시키는 것은 직접해야함. 따라서 BIOS가 USB에 할당한 드라이브 번호와 물리적인 정보를 먼저 알아야함. BIOS가 알려줌.

BIOS는 POST의 마지막 단계에서 부트 로더로 제어를 넘기면서 DL 레지스터에 어떤 정보를 넘겨주는데, 이 정보가 바로 부트로더가 로딩된 디바이스의 번호임.

## 플로피 디스크
- 헤드 : 디스크 표면 앞,뒤
- 트랙 : 동심원
- 섹터 : 트랙을 이루는 조각들, 언급한대로 512바이트임.

### 플로피 디스크의 논리적 순서
섹터(1\~18)->헤드(0\~1)->트랙(0\~79) 순으로 읽음.

## 함수 구현
### 호출 규약
```C
int ADD( int i00, int i01, int i02) {
    return i00 + i01 + i02;
}

int main(void) {
    int i_return = ADD(0,1,2);
}
```
#### stdcall
- 파라미터를 스택에 넣을 때 오른쪽에서 왼쪽 순서로 넣음.
- 반환값은 eax
- 호출 종료 후 파라미터를 callee가 처리함.
#### cdecl
- 파라미터를 오른쪽에서 왼쪽 순서로 넣음.
- 반환값은 eax
- 호출 종류 후 파라미터를 caller가 처리함.

## USB에 부트로더 담기.
1. disk.img 파일을 dd로 usb에 옮김
```
sudo umount /dev/sda
sudo dd if=disk.img of=/dev/sda bs=4M conv=fsync
sudo sync
```
2. 파티션 테이블 설정

플로피 디스크와는 다르게, USB는 하드디스크로 인식되므로 파티션 테이블을 설정해주어야함.
```
sudo sfdisk -d /dev/sda
```
파티션 테이블 설정 여부 확인
```
echo ',,83,*' | sudo sfdisk /dev/sda
```

```
<start>, <size>, <type>, <flags>
```
- start가 비어있다 -> 알아서 정해라. (2048번 섹터가 정배임.)
- size가 비어있다 -> 최대한
- 83 -> 리눅스 파일시스템
- flags에 * -> 이 파티션은 부트가능함.

파티션을 만들어줌.
