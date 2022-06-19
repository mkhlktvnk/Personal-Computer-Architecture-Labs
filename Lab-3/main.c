#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

// 0x40 - нулевой канал
// 0x41 - первый канал
// 0х42 - второй канал
// 0x43 - управляющий канал
// 0x61 - канал управления динамиком

#define MAX_TIMER_FREQ 1193180
void set_timer_count(int delim) {
	char value = MAX_TIMER_FREQ / delim;
	outp(0x42, (char)value);              // записываем младшие биты
	outp(0x42, (char)(value >> 8));       // записываем старшие биты
}
#undef MAX_TIMER_FREQ

// 0x61 - регистр динамика 
// бит 1 отвечает за то, включен звук или нет
void turn_sound(int status) {
	(status == 1) ? 
    outp(0x61, inp(0x61) | 3) :         	    // включить звук
		outp(0x61, inp(0x61) & 0xFFFC);     // выключить звук
}

void play_sound(int frequencies[], int delays[], int number) {
	for (int i = 0; i < number; i++) {
		turn_sound(1);
		set_timer_count(frequencies[i]);
		delay(delays[i]);
		turn_sound(0);
	}
}

char get_status_word(char port, char cmd) {
	outp(0x43, cmd);
	return inp(port);
}

#define MAX_16_BIT_VAL 65535
long get_delim(char port) {
	long delim = 0;
	long delim_max = 0;
	for (long i = 0; i < MAX_16_BIT_VAL; i++) {
		long delim_low = inp(port);
		long delim_high = inp(port);

		delim = delim_high << 8 | delim_low;

		if (delim_max < delim) {
			delim_max = delim;
		}
	}
	return delim_max;
}
#undef MAX_16_BIT_VAL


int main() {
	char buffer[9];
	int frequencies[] = { 293, 392, 293, 392, 523, 659, 880, 784, 698 };
	int delays[] = { 200, 400, 200, 400, 200, 200, 400, 200, 200 };

	system("cls");

	// playing sounds
	printf("Sound is playing...\n\n");
  
  /* в регистр управления - 1011 0101: 
	(10) второй канал, 
    	(11) чтение/запись младший-старший,
    	(010) режим генератора импульсов
	(1) двоично-десятичный формат 
  */
	outp(0x43, 0xB6);                            
	play_sound(frequencies, delays, 9);

	// status words output
  
  /*в регистр управления - 1110 0010
    (11) считывание значений из регистров каналов,
    (1)  значение счетчика не фиксируется
    (0)  получение статуса выбранного канала
    (001) нулевой канал
    (0) нулевой бит не используется и должен быть установлен в 0
  */
	char reg = get_status_word(0x40, 0xE2);
	itoa(reg, buffer, 2);
	printf("0x40 channel status word: %s\n", buffer);
  

  /*в регистр управления - 1110 0100
    (11) считывание значений из регистров каналов,
    (1)  значение счетчика не фиксируется
    (0)  получение статуса выбранного канала
    (010) первый канал
    (0) нулевой бит не используется и должен быть установлен в 0
  */
	reg = get_status_word(0x41, 0xE4);
	itoa(reg, buffer, 2);
	printf("0x41 channel status word: %s\n", buffer);
  
  /*в регистр управления - 1110 1000
    (11) считывание значений из регистров каналов,
    (1)  значение счетчика не фиксируется
    (0)  получение статуса выбранного канала
    (100) второй канал
    (0) нулевой бит не используется и должен быть установлен в 0
  */
	reg = get_status_word(0x42, 0xE8);
	itoa(reg, buffer, 2);
	printf("0x42 channel status word: %s\n\n", buffer);

	// delims output
	outp(0x43, 0x00); // cmd = 0000 0000 (0-й канал)
	long delim = get_delim(0x40);
	printf("0x40 channel delim: %X\n", delim);

	outp(0x43, 0x40); // cmd = 0100 0000 (1-й канал)
	delim = get_delim(0x41);
	printf("0x41 channel delim: %X\n", delim);

	outp(0x43, 0x80);  // cmd = 1000 0000 (2-й канал)
	delim = get_delim(0x42);
	printf("0x42 channel delim: %X\n\n", delim);

	system("pause");
	return 0;
}
