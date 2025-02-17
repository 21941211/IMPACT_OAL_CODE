#include <HDC2080.h>
#include <Wire.h>
#include "pinmapping.h"
#include <stdlib.h>
#include <vector>
#include "MeasureFlags.h"
#include "SDI-12_Driver.h"
#include <cmath>


#define ADDR 0x40
#define ADDR2 0x41

void SFSetup();
void SF_Measure();
void testRead();