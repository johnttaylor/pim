#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "rgbled.hpp"

using namespace pimoroni;

ST7789 st7789(PicoDisplay::WIDTH, PicoDisplay::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

RGBLED led(PicoDisplay::LED_R, PicoDisplay::LED_G, PicoDisplay::LED_B);


int main() {
  st7789.set_backlight(200);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.x = rand() % 240;
    shape.y = rand() % 135;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 128.0f;
    shape.dy = float(rand() % 255) / 128.0f;
    shape.pen = graphics.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  uint32_t i = 0;
  Pen BG = graphics.create_pen(120, 40, 60);
  Pen YELLOW = graphics.create_pen(255, 255, 0);
  Pen TEAL = graphics.create_pen(0, 255, 255);
  Pen WHITE = graphics.create_pen(255, 255, 255);

  while(true) {
    graphics.set_pen(BG);
    graphics.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if(shape.x < 0) shape.dx *= -1;
      if(shape.x >= graphics.bounds.w) shape.dx *= -1;
      if(shape.y < 0) shape.dy *= -1;
      if(shape.y >= graphics.bounds.h) shape.dy *= -1;

      graphics.set_pen(shape.pen);
      graphics.circle(Point(shape.x, shape.y), shape.r);
    }

    float led_step = fmod(i / 20.0f, M_PI * 2.0f);
    int r = (sin(led_step) * 32.0f) + 32.0f;
    led.set_rgb(r, r / 1.2f, r);


    std::vector<Point> poly;
    poly.push_back(Point(30, 30));
    poly.push_back(Point(50, 35));
    poly.push_back(Point(70, 25));
    poly.push_back(Point(80, 65));
    poly.push_back(Point(50, 85));
    poly.push_back(Point(30, 45));

    graphics.set_pen(YELLOW);
    //pico_display.pixel(Point(0, 0));
    graphics.polygon(poly);

    graphics.set_pen(TEAL);
    graphics.triangle(Point(50, 50), Point(130, 80), Point(80, 110));

    graphics.set_pen(WHITE);
    graphics.line(Point(50, 50), Point(120, 80));
    graphics.line(Point(20, 20), Point(120, 20));
    graphics.line(Point(20, 20), Point(20, 120));

    for(int r = 0; r < 30; r++) {
      for(int j = 0; j < 10; j++) {
        float rads = ((M_PI * 2) / 30.0f) * float(r);
        rads += (float(i) / 100.0f);
        rads += (float(j) / 100.0f);
        float cx = sin(rads) * 300.0f;
        float cy = cos(rads) * 300.0f;
        graphics.line(Point(120, 67), Point(cx + 120, cy + 67));
      }
    }

    // update screen
    st7789.update(&graphics);
    sleep_ms(1000 / 60);
    i++;
  }

    return 0;
}
