# 보너스: 문틀 장착시 인식률 개선
## 개요
Roode의 개발 컨셉은 상단 설치지만 대한민국의 대부분 가정집은 천장이 높지 않습니다. 무릎 높이 정도에 설치하는 경우가 많은데, 문의 폭이 1m 를 넘지 않으므로 센싱 거리를 짧게 설정하면 반응 속도가 빨라져 인식률이 개선됩니다.


## ESPHome 자동화 설정
```yaml
vl53l1x:
  (중략)
  calibration:
    ranging: shortest
  (중략)

```

## Ranging 목록

| Ranging | 최대 거리 | 반응 속도 | 비고 |
| - | -: | -: | - |
| shortest | 1.3m | 15ms | |
| short | 1.3m | 20ms | |
| medium | 4m | 33ms | |
| long | 4m | 50ms | |
| longer | 4m | 100ms | |
| longest | 4m | 200ms | |



# 출처
- <https://github.com/Lyr3x/Roode>
- <https://www.st.com/resource/en/user_manual/dm00562924-a-guide-to-using-the-vl53l1x-ultra-lite-driver-stmicroelectronics.pdf>
