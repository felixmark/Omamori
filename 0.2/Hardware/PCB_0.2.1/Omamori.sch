EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR09
U 1 1 605032B2
P 3450 4250
F 0 "#PWR09" H 3450 4000 50  0001 C CNN
F 1 "GND" H 3455 4077 50  0000 C CNN
F 2 "" H 3450 4250 50  0001 C CNN
F 3 "" H 3450 4250 50  0001 C CNN
	1    3450 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 4250 3450 4200
Wire Wire Line
	3850 4200 3850 4250
$Comp
L power:GND #PWR011
U 1 1 6050306E
P 3850 4250
F 0 "#PWR011" H 3850 4000 50  0001 C CNN
F 1 "GND" H 3855 4077 50  0000 C CNN
F 2 "" H 3850 4250 50  0001 C CNN
F 3 "" H 3850 4250 50  0001 C CNN
	1    3850 4250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C_super1
U 1 1 6058E241
P 4300 2600
F 0 "C_super1" H 4415 2646 50  0000 L CNN
F 1 "640mF" H 4415 2555 50  0000 L CNN
F 2 "own_footprints:Supercap" H 4338 2450 50  0001 C CNN
F 3 "~" H 4300 2600 50  0001 C CNN
	1    4300 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 2600 4050 2450
Wire Wire Line
	4050 2450 4300 2450
$Comp
L power:GND #PWR012
U 1 1 604DE9F8
P 4300 2750
F 0 "#PWR012" H 4300 2500 50  0001 C CNN
F 1 "GND" H 4305 2577 50  0000 C CNN
F 2 "" H 4300 2750 50  0001 C CNN
F 3 "" H 4300 2750 50  0001 C CNN
	1    4300 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2050 3250 2600
$Comp
L Device:C C_hvr1
U 1 1 609CF2BF
P 2500 2200
F 0 "C_hvr1" H 2615 2246 50  0000 L CNN
F 1 "4.7uF" H 2615 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2538 2050 50  0001 C CNN
F 3 "~" H 2500 2200 50  0001 C CNN
	1    2500 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 609CF779
P 2500 2350
F 0 "#PWR04" H 2500 2100 50  0001 C CNN
F 1 "GND" H 2505 2177 50  0000 C CNN
F 2 "" H 2500 2350 50  0001 C CNN
F 3 "" H 2500 2350 50  0001 C CNN
	1    2500 2350
	1    0    0    -1  
$EndComp
Connection ~ 3250 2050
$Comp
L Device:R R_oc1_2
U 1 1 609D50F7
P 2500 2950
F 0 "R_oc1_2" H 2570 2996 50  0000 L CNN
F 1 "10M" H 2570 2905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2430 2950 50  0001 C CNN
F 3 "~" H 2500 2950 50  0001 C CNN
	1    2500 2950
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R_oc2
U 1 1 609D577E
P 3050 2300
F 0 "R_oc2" H 3120 2346 50  0000 L CNN
F 1 "4.3M" H 3120 2255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2980 2300 50  0001 C CNN
F 3 "~" H 3050 2300 50  0001 C CNN
	1    3050 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 609DECA8
P 2300 2950
F 0 "#PWR06" H 2300 2700 50  0001 C CNN
F 1 "GND" H 2305 2777 50  0000 C CNN
F 2 "" H 2300 2950 50  0001 C CNN
F 3 "" H 2300 2950 50  0001 C CNN
	1    2300 2950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 609CB8A4
P 850 3650
F 0 "#PWR01" H 850 3400 50  0001 C CNN
F 1 "GND" H 855 3477 50  0000 C CNN
F 2 "" H 850 3650 50  0001 C CNN
F 3 "" H 850 3650 50  0001 C CNN
	1    850  3650
	1    0    0    -1  
$EndComp
$Comp
L Omamori-rescue:KXOB25-05X3F-own_symbols Sol_1
U 1 1 609C9A98
P 850 2500
F 0 "Sol_1" V 996 2422 50  0000 R CNN
F 1 "KXOB25-05X3F" V 905 2422 50  0001 R CNN
F 2 "own_footprints:KXOB25-05X3F" H 650 2350 50  0001 C CNN
F 3 "" H 650 2350 50  0001 C CNN
	1    850  2500
	0    -1   -1   0   
$EndComp
$Comp
L Omamori-rescue:KXOB25-05X3F-own_symbols Sol_2
U 1 1 609C8E61
P 850 3300
F 0 "Sol_2" V 996 3222 50  0000 R CNN
F 1 "KXOB25-05X3F" V 905 3222 50  0001 R CNN
F 2 "own_footprints:KXOB25-05X3F" H 650 3150 50  0001 C CNN
F 3 "" H 650 3150 50  0001 C CNN
	1    850  3300
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C_ref1
U 1 1 609EC74D
P 2900 4250
F 0 "C_ref1" H 3015 4296 50  0000 L CNN
F 1 "0.01uF" H 3015 4205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 2938 4100 50  0001 C CNN
F 3 "~" H 2900 4250 50  0001 C CNN
	1    2900 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 609F3E2A
P 2900 4400
F 0 "#PWR07" H 2900 4150 50  0001 C CNN
F 1 "GND" H 2905 4227 50  0000 C CNN
F 2 "" H 2900 4400 50  0001 C CNN
F 3 "" H 2900 4400 50  0001 C CNN
	1    2900 4400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 609F652D
P 3050 3800
F 0 "#PWR08" H 3050 3550 50  0001 C CNN
F 1 "GND" H 3055 3627 50  0000 C CNN
F 2 "" H 3050 3800 50  0001 C CNN
F 3 "" H 3050 3800 50  0001 C CNN
	1    3050 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2050 3350 2050
Wire Wire Line
	3650 2050 3750 2050
Wire Wire Line
	3750 2050 3750 2600
Wire Wire Line
	2900 3700 2900 4100
Wire Wire Line
	2900 3700 3050 3700
$Comp
L Device:R R_ok1
U 1 1 60A15444
P 1900 3950
F 0 "R_ok1" H 1970 3996 50  0000 L CNN
F 1 "3.3M" H 1970 3905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 1830 3950 50  0001 C CNN
F 3 "~" H 1900 3950 50  0001 C CNN
	1    1900 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R_ok2
U 1 1 60A15932
P 1900 3650
F 0 "R_ok2" H 1970 3696 50  0000 L CNN
F 1 "2.7M" H 1970 3605 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 1830 3650 50  0001 C CNN
F 3 "~" H 1900 3650 50  0001 C CNN
	1    1900 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R_ok3
U 1 1 60A15D6C
P 1900 3350
F 0 "R_ok3" H 1970 3396 50  0000 L CNN
F 1 "3.3M" H 1970 3305 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 1830 3350 50  0001 C CNN
F 3 "~" H 1900 3350 50  0001 C CNN
	1    1900 3350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 60A50428
P 1900 4100
F 0 "#PWR02" H 1900 3850 50  0001 C CNN
F 1 "GND" H 1905 3927 50  0000 C CNN
F 2 "" H 1900 4100 50  0001 C CNN
F 3 "" H 1900 4100 50  0001 C CNN
	1    1900 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3200 2200 3200
Wire Wire Line
	1900 3500 3050 3500
Connection ~ 1900 3500
Wire Wire Line
	2250 3600 2250 3800
Wire Wire Line
	2250 3800 1900 3800
Wire Wire Line
	2250 3600 3050 3600
Connection ~ 1900 3800
$Comp
L Device:R R_ov1
U 1 1 60A51EC7
P 2200 4150
F 0 "R_ov1" H 2270 4196 50  0000 L CNN
F 1 "6.2M" H 2270 4105 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2130 4150 50  0001 C CNN
F 3 "~" H 2200 4150 50  0001 C CNN
	1    2200 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R_ov2
U 1 1 60A5234E
P 2200 4450
F 0 "R_ov2" H 2270 4496 50  0000 L CNN
F 1 "3.3M" H 2270 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2130 4450 50  0001 C CNN
F 3 "~" H 2200 4450 50  0001 C CNN
	1    2200 4450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R_uv1
U 1 1 60A528F0
P 2550 4450
F 0 "R_uv1" H 2620 4496 50  0000 L CNN
F 1 "5.6M" H 2620 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2480 4450 50  0001 C CNN
F 3 "~" H 2550 4450 50  0001 C CNN
	1    2550 4450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R_uv2
U 1 1 60A52DB8
P 2550 4150
F 0 "R_uv2" H 2620 4196 50  0000 L CNN
F 1 "4.3M" H 2620 4105 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2480 4150 50  0001 C CNN
F 3 "~" H 2550 4150 50  0001 C CNN
	1    2550 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 609C1CBB
P 2200 4600
F 0 "#PWR03" H 2200 4350 50  0001 C CNN
F 1 "GND" H 2205 4427 50  0000 C CNN
F 2 "" H 2200 4600 50  0001 C CNN
F 3 "" H 2200 4600 50  0001 C CNN
	1    2200 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 609C2C1D
P 2550 4600
F 0 "#PWR05" H 2550 4350 50  0001 C CNN
F 1 "GND" H 2555 4427 50  0000 C CNN
F 2 "" H 2550 4600 50  0001 C CNN
F 3 "" H 2550 4600 50  0001 C CNN
	1    2550 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 4000 2200 3200
Connection ~ 2200 3200
Wire Wire Line
	2200 3200 2550 3200
Wire Wire Line
	2200 4300 2350 4300
Wire Wire Line
	2350 4300 2350 3300
Wire Wire Line
	2350 3300 3050 3300
Connection ~ 2200 4300
Wire Wire Line
	2550 4000 2550 3200
Connection ~ 2550 3200
Wire Wire Line
	2550 3200 3050 3200
Wire Wire Line
	2550 4300 2700 4300
Wire Wire Line
	2700 4300 2700 3400
Wire Wire Line
	2700 3400 3050 3400
Connection ~ 2550 4300
$Comp
L power:GND #PWR010
U 1 1 609D67E9
P 3650 4250
F 0 "#PWR010" H 3650 4000 50  0001 C CNN
F 1 "GND" H 3655 4077 50  0000 C CNN
F 2 "" H 3650 4250 50  0001 C CNN
F 3 "" H 3650 4250 50  0001 C CNN
	1    3650 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 4200 3650 4250
$Comp
L Regulator_Switching:TPS61222DCK U2
U 1 1 609D9F4F
P 6300 3700
F 0 "U2" H 6300 4067 50  0000 C CNN
F 1 "TPS61222DCK" H 6300 3976 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:Texas_R-PDSO-G6" H 6300 2900 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tps61220.pdf" H 6300 3550 50  0001 C CNN
	1    6300 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 3600 6900 3600
Wire Wire Line
	6700 3800 6700 3600
Connection ~ 6700 3600
Wire Wire Line
	5600 3600 5600 3700
$Comp
L Device:C C_post_boost1
U 1 1 60A258CF
P 6900 3750
F 0 "C_post_boost1" H 7015 3796 50  0000 L CIN
F 1 "10uF" H 7015 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 6938 3600 50  0001 C CNN
F 3 "~" H 6900 3750 50  0001 C CNN
	1    6900 3750
	1    0    0    -1  
$EndComp
Connection ~ 6900 3600
Wire Wire Line
	6900 4000 6900 3900
Wire Wire Line
	9150 3000 9150 2900
Wire Wire Line
	9050 2800 9050 3000
$Comp
L Device:R R_data1
U 1 1 60A307A3
P 9750 2900
F 0 "R_data1" V 9543 2900 50  0000 C CNN
F 1 "500" V 9634 2900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9680 2900 50  0001 C CNN
F 3 "~" H 9750 2900 50  0001 C CNN
	1    9750 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	9600 2900 9150 2900
$Comp
L Connector:Conn_01x04_Female J1
U 1 1 60A1387B
P 8850 950
F 0 "J1" V 8788 662 50  0000 R CNN
F 1 "Conn_01x04_Female" V 8697 662 50  0000 R CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_1x04_P1.27mm_Horizontal" H 8850 950 50  0001 C CNN
F 3 "~" H 8850 950 50  0001 C CNN
	1    8850 950 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5600 3700 5850 3700
Text Label 1950 2050 0    50   ~ 0
Vsolar
Text Label 8000 3600 0    50   ~ 0
Vcc_ATTiny
Text Label 2550 3200 0    50   ~ 0
Vr_div
Text Label 8050 1800 0    50   ~ 0
Vmeasure
Text Label 8950 1550 0    50   ~ 0
Tx
Text Label 8850 1550 0    50   ~ 0
Rx
Text Label 10200 2800 0    50   ~ 0
Vcc_LED
Text Label 9300 2900 0    50   ~ 0
Data_LED
$Comp
L Device:L L_boost1
U 1 1 60A0F419
P 5750 3600
F 0 "L_boost1" V 5940 3600 50  0000 C CNN
F 1 "4.7uH" V 5849 3600 50  0000 C CNN
F 2 "Inductor_SMD:L_Wuerth_MAPI-3015" H 5750 3600 50  0001 C CNN
F 3 "~" H 5750 3600 50  0001 C CNN
	1    5750 3600
	0    -1   -1   0   
$EndComp
$Comp
L Device:L L_bst1
U 1 1 609CFCA3
P 3500 2050
F 0 "L_bst1" V 3690 2050 50  0000 C CNN
F 1 "22uH" V 3599 2050 50  0000 C CNN
F 2 "Inductor_SMD:L_Wuerth_MAPI-3015" H 3500 2050 50  0001 C CNN
F 3 "~" H 3500 2050 50  0001 C CNN
	1    3500 2050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8950 1800 8950 3000
Wire Wire Line
	7800 1800 7950 1800
Wire Wire Line
	8850 3000 8850 1150
Wire Wire Line
	8950 1150 8950 1650
Wire Wire Line
	8950 1650 8750 1650
Wire Wire Line
	8750 1650 8750 3000
Text Label 9300 3000 0    50   ~ 0
Reset
Wire Wire Line
	9250 3000 9650 3000
$Comp
L own_symbols:TS-1233 U4
U 1 1 60ACC29B
P 9700 3200
F 0 "U4" V 9746 3072 50  0000 R CNN
F 1 "TS-1233" V 9655 3072 50  0000 R CNN
F 2 "own_footprints:TS-1233" H 9700 3050 50  0001 C CNN
F 3 "" H 9700 3050 50  0001 C CNN
	1    9700 3200
	0    -1   -1   0   
$EndComp
NoConn ~ 9650 3400
NoConn ~ 9750 3000
$Comp
L Battery_Management:BQ25504 U1
U 1 1 6049F0BD
P 3650 3400
F 0 "U1" H 3500 4150 50  0000 L CNN
F 1 "BQ25504" H 4294 3355 50  0000 L CNN
F 2 "footprints:VQFN-16-1EP_3x3mm_P0.5mm_EP1.8x1.8mm" H 3650 3400 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/bq25504.pdf" H 3350 4200 50  0001 C CNN
	1    3650 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R_oc1
U 1 1 60AEA81A
P 2850 2950
F 0 "R_oc1" H 2920 2996 50  0000 L CNN
F 1 "5.6M" H 2920 2905 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 2780 2950 50  0001 C CNN
F 3 "~" H 2850 2950 50  0001 C CNN
	1    2850 2950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3000 2950 3050 2950
Connection ~ 3050 2950
Wire Wire Line
	3050 2950 3050 3100
Wire Wire Line
	2700 2950 2650 2950
Wire Wire Line
	2350 2950 2300 2950
Wire Wire Line
	5900 3800 5850 3800
Connection ~ 5600 3700
$Comp
L power:GND #PWR014
U 1 1 609F4F40
P 4250 4000
F 0 "#PWR014" H 4250 3750 50  0001 C CNN
F 1 "GND" H 4255 3827 50  0000 C CNN
F 2 "" H 4250 4000 50  0001 C CNN
F 3 "" H 4250 4000 50  0001 C CNN
	1    4250 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C_pre_boost1
U 1 1 609F0244
P 5600 3850
F 0 "C_pre_boost1" H 5715 3896 50  0000 L CNN
F 1 "10uF" H 5715 3805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5638 3700 50  0001 C CNN
F 3 "~" H 5600 3850 50  0001 C CNN
	1    5600 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C_filter1
U 1 1 609F4440
P 4250 3850
F 0 "C_filter1" H 4150 3950 50  0000 L CNN
F 1 "0.1uF" H 4200 3750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 4288 3700 50  0001 C CNN
F 3 "~" H 4250 3850 50  0001 C CNN
	1    4250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3700 5600 3700
Wire Wire Line
	3050 2150 3050 2050
Connection ~ 3050 2050
Wire Wire Line
	3050 2050 3250 2050
Wire Wire Line
	3050 2450 3050 2950
Connection ~ 2500 2050
Wire Wire Line
	2500 2050 3050 2050
Wire Wire Line
	850  2050 2500 2050
Text Label 4350 3100 0    50   ~ 0
Vbat_ok
Wire Wire Line
	5850 3800 5850 3700
Connection ~ 5850 3700
Wire Wire Line
	5850 3700 5900 3700
$Comp
L Device:R R_oc3
U 1 1 61C235D3
P 7650 1800
F 0 "R_oc3" H 7720 1846 50  0000 L CNN
F 1 "1.8M" H 7720 1755 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 7580 1800 50  0001 C CNN
F 3 "~" H 7650 1800 50  0001 C CNN
	1    7650 1800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R_oc5
U 1 1 61C24BDB
P 7950 2300
F 0 "R_oc5" H 8020 2346 50  0000 L CNN
F 1 "10M" H 8020 2255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 7880 2300 50  0001 C CNN
F 3 "~" H 7950 2300 50  0001 C CNN
	1    7950 2300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R_oc4
U 1 1 61C266D9
P 7950 2000
F 0 "R_oc4" H 8020 2046 50  0000 L CNN
F 1 "8.2M" H 8020 1955 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" V 7880 2000 50  0001 C CNN
F 3 "~" H 7950 2000 50  0001 C CNN
	1    7950 2000
	-1   0    0    1   
$EndComp
Wire Wire Line
	7950 1850 7950 1800
Wire Wire Line
	6900 3600 8450 3600
Wire Wire Line
	6900 1150 6900 3600
Wire Wire Line
	6900 1150 8750 1150
Wire Wire Line
	7500 1800 5250 1800
Wire Wire Line
	5250 1800 5250 3700
Wire Wire Line
	7950 1800 8950 1800
Connection ~ 7950 1800
Wire Wire Line
	10100 2900 9900 2900
Wire Wire Line
	9750 3400 9750 3600
Wire Wire Line
	9650 3600 9750 3600
Connection ~ 9750 3600
Wire Wire Line
	9050 2800 10400 2800
Text Notes 5650 3150 0    79   ~ 0
5V Boost Converter\n
Text Notes 7300 1450 0    118  ~ 0
Voltage Divider\n
Text Notes 8450 850  0    118  ~ 0
Serial Pins\n
Text Notes 8750 4650 0    118  ~ 0
AT Tiny\n
Text Notes 10050 4650 0    118  ~ 0
RGBW LED
Text Notes 2600 1600 0    118  ~ 0
Solar charge controller
Text Notes 550  1850 0    118  ~ 0
Photovoltaic
$Comp
L power:GND #PWR0101
U 1 1 61C12810
P 4450 4000
F 0 "#PWR0101" H 4450 3750 50  0001 C CNN
F 1 "GND" H 4455 3827 50  0000 C CNN
F 2 "" H 4450 4000 50  0001 C CNN
F 3 "" H 4450 4000 50  0001 C CNN
	1    4450 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C_Stor1
U 1 1 61C12816
P 4450 3850
F 0 "C_Stor1" H 4500 3950 50  0000 L CNN
F 1 "10uF" H 4450 3750 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 4488 3700 50  0001 C CNN
F 3 "~" H 4450 3850 50  0001 C CNN
	1    4450 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 3700 4450 3700
Connection ~ 4250 3700
Connection ~ 4300 2450
Wire Wire Line
	5250 1800 4300 1800
Wire Wire Line
	4300 1800 4300 2450
Connection ~ 5250 1800
$Comp
L MCU_Microchip_ATtiny:ATtiny13A-SSU U3
U 1 1 604F4615
P 9050 3600
F 0 "U3" V 8475 3600 50  0000 C CNN
F 1 "ATtiny85" V 8384 3600 50  0000 C CNN
F 2 "Package_SO:SOIC-8_5.275x5.275mm_P1.27mm" H 9050 3600 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/doc8126.pdf" H 9050 3600 50  0001 C CNN
	1    9050 3600
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR013
U 1 1 60A0DF2C
P 4800 4400
F 0 "#PWR013" H 4800 4150 50  0001 C CNN
F 1 "GND" H 4805 4227 50  0000 C CNN
F 2 "" H 4800 4400 50  0001 C CNN
F 3 "" H 4800 4400 50  0001 C CNN
	1    4800 4400
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:IRLML0030 Q1
U 1 1 61BDECD2
P 5000 4300
F 0 "Q1" H 5204 4346 50  0000 L CNN
F 1 "IRLML0030" H 5204 4255 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5200 4225 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml0030pbf.pdf?fileId=5546d462533600a401535664773825df" H 5000 4300 50  0001 L CNN
	1    5000 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	4250 3100 4700 3100
Wire Wire Line
	5600 4000 6300 4000
Wire Wire Line
	6300 4000 6900 4000
Connection ~ 6300 4000
Wire Wire Line
	6300 4000 6300 4400
Wire Wire Line
	7950 2450 7950 4400
Wire Wire Line
	7950 4400 6300 4400
Wire Wire Line
	9750 4400 7950 4400
Wire Wire Line
	9750 3600 9750 4400
$Comp
L LED:SK6812 D1
U 1 1 604F2786
P 10400 3900
F 0 "D1" H 10744 3946 50  0000 L CNN
F 1 "SK6812" H 10744 3855 50  0000 L CNN
F 2 "LED_SMD:LED_SK6812_PLCC4_5.0x5.0mm_P3.2mm" H 10450 3600 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf" H 10500 3525 50  0001 L TNN
	1    10400 3900
	1    0    0    -1  
$EndComp
NoConn ~ 10700 3900
Text Label 10100 3650 0    50   ~ 0
Din_LED
$Comp
L Device:C C_led1
U 1 1 60A4741C
P 10550 3500
F 0 "C_led1" H 10665 3546 50  0000 L CNN
F 1 "100nF" H 10665 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 10588 3350 50  0001 C CNN
F 3 "~" H 10550 3500 50  0001 C CNN
	1    10550 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10400 3600 10400 3500
Connection ~ 10400 3500
Wire Wire Line
	10700 3500 10950 3500
Wire Wire Line
	10100 3900 10100 2900
Wire Wire Line
	10400 4200 10400 4400
Wire Wire Line
	10400 4400 9750 4400
Wire Wire Line
	10400 4400 10950 4400
Wire Wire Line
	10950 3500 10950 4400
Connection ~ 10400 4400
Connection ~ 9750 4400
Wire Wire Line
	10400 2800 10400 3500
Connection ~ 7950 4400
Connection ~ 6300 4400
Wire Wire Line
	5200 4400 6300 4400
Wire Wire Line
	5000 4100 5000 3100
Text Label 5550 4400 0    50   ~ 0
Drain
Wire Wire Line
	9050 1150 9050 1800
Wire Wire Line
	9050 1800 10950 1800
Wire Wire Line
	10950 1800 10950 3500
Connection ~ 10950 3500
$Comp
L Device:R R_pulldown1
U 1 1 61C260A2
P 4700 3300
F 0 "R_pulldown1" H 4770 3346 50  0000 L CNN
F 1 "10M" H 4770 3255 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 4630 3300 50  0001 C CNN
F 3 "~" H 4700 3300 50  0001 C CNN
	1    4700 3300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 61C28559
P 4700 3450
F 0 "#PWR0102" H 4700 3200 50  0001 C CNN
F 1 "GND" H 4705 3277 50  0000 C CNN
F 2 "" H 4700 3450 50  0001 C CNN
F 3 "" H 4700 3450 50  0001 C CNN
	1    4700 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 3150 4700 3100
Connection ~ 4700 3100
Wire Wire Line
	4700 3100 5000 3100
Text Label 4550 1800 0    50   ~ 0
Vbat
$EndSCHEMATC
