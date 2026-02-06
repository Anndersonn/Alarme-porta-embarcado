/*
 * Projeto: Alarme de Porta Aberta (ATmega328P)
 * Autor: José Anderson da Costa Araújo
 * Descrição: Aciona um alarme se a porta ficar aberta por 10 segundos.
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned char segundos = 0;
volatile unsigned char porta_aberta = 0;
volatile unsigned char alarme_ativo = 0;

void timer1_init() {
    // Timer1 no modo CTC, prescaler 1024
    // WGM12 = 1 ? modo CTC (Clear Timer on Compare)
    // CS12 e CS10 = 1 ? prescaler 1024
    TCCR1A = 0b00000000;        // Modo normal (sem PWM)
    TCCR1B = 0b00001101;        // WGM12=1, CS12=1, CS10=1
    OCR1A = 15624;              // Gera interrupção a cada 1 segundo
    TIMSK1 = 0b00000010;        // Habilita interrupção de comparação A
}

void io_init() {
    DDRC = 0b00000100;          // PC2 como saída (LED ou buzzer)
    DDRD = 0b11110011;          // PD2 e PD3 como entrada
    PORTD = 0b00001100;         // Pull-ups ativados em PD2 e PD3
}

void int_init() {
    // Interrupções externas com borda de subida
    EICRA = 0b00001011;         // ISC00=1 (INT0), ISC01=1 (INT0), ISC10=0 (INT1), ISC11=1 (INT1)
    EIMSK = 0b00000011;         // Habilita INT0 e INT1
}

ISR(TIMER1_COMPA_vect) {
    if (porta_aberta && !alarme_ativo) { // Se a porta estiver aberta e o alarme não disparado
        segundos++;
        if (segundos >= 3) {
            PORTC = 0b00000100;   // Liga o alarme
            alarme_ativo = 1;
        }
    }
}

ISR(INT0_vect) {
    // Detecta mudança no estado da porta (PD2)
     porta_aberta = 1;
     segundos = 0;
}

ISR(INT1_vect) {
    // Botão de reset/silêncio (PD3)
    alarme_ativo = 0;
    segundos = 0;
    PORTC = 0b00000000;
}

int main(void) {
    io_init();
    int_init();
    timer1_init();
    sei();   // Habilita interrupções globais

    while (1) {
        // Loop principal — tudo via interrupções
    }
}