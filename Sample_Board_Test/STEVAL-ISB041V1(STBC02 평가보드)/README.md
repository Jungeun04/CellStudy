# STBC02 평가보드

ST사에서 작성한 [UM2185](https://www.st.com/resource/en/user_manual/um2185-getting-started-with-the-stevalisb041v1-liionlipo-battery-power-management-evaluation-board-based-on-stbc02-stmicroelectronics.pdf)문서를 참고하여 작성했습니다.
이 테스트는 필자의 상황에 맞게 수정된 내용과 주관적인 내용이 들어가 있습니다.
잘못된 내용이 있을 수 있으니 정확한 [공식 문서](https://www.st.com/en/evaluation-tools/steval-isb041v1.html#documentation)를 참고하여 주세요.

---

## 평가보드 테스트 절차

### 준비물
재충전 가능 배터리(리튬이온), USB Type Micro 전원공급장치(5V), 오실로스코프와 프로브, 신호발생기 혹은 주기적인 구형파 발생장치(문서에서는 테스트에 필요하지 않네요)
추가+
전류 측정용 멀티미터, 1/2W 33Ω 저항(1/2W 저항을 사용하지 않으면 저항이 탈 수 있습니다. 흔히 사용하는 1/4W 저항을 사용해야 한다면 저항 값을 올리세요)

### 주의사항
배터리는 취급에는 조심해 주세요. 이 평가 보드는 온도 측정 센서가 없습니다(10kΩ으로 고정).
60mA의 저전류로 설정되어 있으나 배터리나 평가보드가 불량일 수 있으므로 배터리의 온도가 높게 올라가지 않게 확인해야 합니다.

### 절차

1. 프로브 1을 J1 커넥터의 pin6(CHG)와 GND에 연결 (충전 상태 확인)
2. 프로브 2를 J10 커넥터의 pin22(BAT_S)와 GND에 연결 (배터리 전압 측정)
3. 오실로스코프 전류 측정 프로브를 배터리 양극에 연결 (작성자는 전류 프로브가 없어 멀티미터를 사용하였습니다. 최대 전류가 60mA이므로 큰 문제가 없을 것으로 판단했습니다.)
4. 방전된 배터리를 J10 커넥터의 pin3와 GND에 연결
5. J9 커넥터의 pin3(IN)과 GND에 5V 전원을 공급(혹은 USB 전원 공급장치 연결)
6. 일정 전압 수준까지 IPRE(6mA)로 충전됨.
7. 33Ω의 1/2W 저항을 J8 커넥터의 pin3(LDO)와 GND에 연결 (부하를 주기위해)
8. 프로브 3을 J8 커넥터의 pin2에 연결하여 3.1V가 출력되는지 확인
9. 프로브 3을 J11 커넥터의 pin2(SYS)에 연결하여 약 5V가 출력되고 있는지 확인. J1 커넥터의 pin2(CEN)를 GND와 연결하면 충전 중지 가능.
10. 충전이 중단되면 프로브 1에 연결된 CHG가 LOW(0V)로 바뀜.
11. CEN에 연결된 GND 선을 제거하고 충전 재개.
12. 배터리 전압이 4.2V에 다다르면 충전 전류 감소(프로브 2에 연결된 BAT_S전압으로 확인 가능)
13. 전류가 설정 전류의 5%미만으로 떨어지면 배터리 충전은 자동적으로 종료
14. 전원공급 장치를 제거
15. J11 커넥터의 pin2(SYS_S)를 프로브 3으로 측정하여 전압의 극성이 바뀌고 배터리 전압과 비슷한지 확인
16. J8 커넥터의 pin3(LDO)에 연결되어 있는 저항 제거
17. 배터리를 제거했다가 다시 연결 후, SYS 와 LOD 에 전원이 공급되지 않는 shutdown mode로 진입했는지 확인


## MCU와 같이 사용하기

STBC02는 몇 개의 전용 핀을 이용하여 MCU와 상호작용하며 충전, 전원을 관리 할 수 있습니다.  
  
### us를 만들기위한 함수
STBC02 의 내부 설정을 하기 위해서는 us단위의 대기 시간이 필요하다.  
하지만 HAL Driver에서는 ms만 제공하므로 직접 구현해야한다.
```C

/* Define the MPU frequency (in Hz) */
#define SYSTEM_CLOCK 168000000 // 168MHz

/* Delay function in microseconds */
void delay_us(uint32_t us) {
    uint32_t count = us * (SYSTEM_CLOCK / 21000000);
    for(uint32_t i = 0; i < count; i++) {
        __NOP(); // 빈 명령어를 사용하여 딜레이 생성
    }
}

```
10us 이하에서는 더욱 오차가 심하겠지만 보통 Delay 기능들은 높은 정확도를 요구하지는 않는다.
