#include "PixelShader.h"
#include "EmbemddedFile.h"

void PixelShader::file_read(byte* buf, int size_of_buf) {
  for (int i = 0; i < size_of_buf; ++i) buf[i] = *file++;
}

PixelShader::PixelShader(Vector light0) : Height(240), Width(320), _height(180), _width(240), minZ(-32768) {
  backBuffer = new uint16_t*[_height];
  for (int i = 0; i < _height; ++i) backBuffer[i] = new uint16_t[_width];
  for (int i = 0; i < _height; ++i) for (int j = 0; j < _width; ++j) backBuffer[i][j] = 0;
  zBuffer = new int16_t*[_height];
  for (int i = 0; i < _height; ++i) zBuffer[i] = new int16_t[_width];
  for (int i = 0; i < _height; ++i) for (int j = 0; j < _width; ++j) zBuffer[i][j] = minZ;
  light = light0;
  material.color = WHITE;
}

PixelShader::~PixelShader() {
  for (int i = 0; i < _height; ++i) delete[] backBuffer[i];
  for (int i = 0; i < _height; ++i) delete[] zBuffer[i];
  delete[] backBuffer;
  delete[] zBuffer;
}

void PixelShader::Flip() {
  for (int i = 0; i < _height; ++i) M5.Lcd.drawBitmap((Width - _width)/2, (Height - _height)/2 + i, _width, 1, (uint8_t*)(backBuffer[i]));
}

void PixelShader::Clear(uint16_t color) {
 for (int i = 0; i < _height; ++i) for (int j = 0; j < _width; ++j) backBuffer[i][j] = color;
 for (int i = 0; i < _height; ++i) for (int j = 0; j < _width; ++j) zBuffer[i][j] = minZ;
}

void PixelShader::CreateNormal(Polygon* polygon) {
    float P1x = polygon->vertices[1].x - polygon->vertices[0].x;
    float P1y = polygon->vertices[1].y - polygon->vertices[0].y;
    float P1z = polygon->vertices[1].z - polygon->vertices[0].z;
    float P2x = polygon->vertices[2].x - polygon->vertices[0].x;
    float P2y = polygon->vertices[2].y - polygon->vertices[0].y;
    float P2z = polygon->vertices[2].z - polygon->vertices[0].z;
    float OPx = P1y*P2z - P1z*P2y;
    float OPy = P1z*P2x - P1x*P2z;
    float OPz = P1x*P2y - P1y*P2x;
    Vector v(OPx, OPy, OPz);
    Affine::Normalize(&v);
    for (int i =  0; i < 3; ++i) polygon->normal[i] = v;
}

uint16_t DecayBrightness(uint16_t color, float rate){
  rate = fabs(rate);
  if (rate < 0) rate = 0;
  if (rate > 1) rate = 1;
  byte r = 0x1F & (color >> 11);
  byte g = 0x3F & (color >> 5);
  byte b = 0x1F & (color >> 0);
  r = (byte)(rate * (float)r);
  g = (byte)(rate * (float)g);
  b = (byte)(rate * (float)b);
  return (r << 11) | (g << 5) | (b << 0);
}

bool Culling(Polygon* polygon) {
    float P1x = polygon->vertices[1].x - polygon->vertices[0].x;
    float P1y = polygon->vertices[1].y - polygon->vertices[0].y;
    float P2x = polygon->vertices[2].x - polygon->vertices[0].x;
    float P2y = polygon->vertices[2].y - polygon->vertices[0].y;
    float OPz = P1x*P2y - P1y*P2x;
    return OPz < 0;
}

void PixelShader::DrawPolygon(Polygon* polygon, bool gouraud, bool texture) {
  //if(Culling(polygon)) return;
  if (!gouraud) {
    CreateNormal(polygon);
    plainColor = DecayBrightness(material.color, -Affine::Dot(&light, &(polygon->normal[0])));
  }
  if (!textureFileAvailable) texture = false;
  Point p[3];
  for (int i = 0; i < 3; ++i) p[i] = polygon->vertices[i].ToPoint();
  int32_t x0 = p[0].X;
  int32_t y0 = p[0].Y;
  int32_t x1 = p[1].X;
  int32_t y1 = p[1].Y;
  int32_t x2 = p[2].X;
  int32_t y2 = p[2].Y;
  int32_t a, b, y, last;
  if (y0 > y1) {swap_coord(y0, y1); swap_coord(x0, x1);}
  if (y1 > y2) {swap_coord(y2, y1); swap_coord(x2, x1);}
  if (y0 > y1) {swap_coord(y0, y1); swap_coord(x0, x1);}
  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)      a = x1;
    else if (x1 > b) b = x1;
    if (x2 < a)      a = x2;
    else if (x2 > b) b = x2;
    DrawHLine(a, y0, b - a + 1, polygon, gouraud, texture);
    return;
  }
  int32_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1,sa = 0, sb = 0;
  if (y1 == y2) last = y1;  // Include y1 scanline
  else         last = y1 - 1; // Skip it
  for (y = y0; y <= last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    if (a > b) swap_coord(a, b);
    DrawHLine(a, y, b - a + 1, polygon, gouraud, texture);
  }
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    if (a > b) swap_coord(a, b);
    DrawHLine(a, y, b - a + 1, polygon, gouraud, texture);
  }
}

// for test
void CountAverageLineWidth(int32_t w) {
  static long t = 0;
  static long w_sum = 0;
  static int max_t = 1000000;
  if (t < max_t) {
    ++t;
    w_sum += w;
    if (t == max_t) M5.Lcd.println((float)w_sum/(float)max_t);
  }  
}

void PixelShader::DrawHLine(int32_t x, int32_t y, int32_t w, Polygon* polygon, bool gouraud, bool texture) {
  if (x < 0) {
    w += x;
    x = 0;
  }
  if ((x >= _width) || (y >= _height) || (y < 0) || (w < 1)) return;
  if ((x + w - 1) >= _width)  w = _width - x;
  Vector U(polygon->vertices[1].x - polygon->vertices[0].x, polygon->vertices[1].y - polygon->vertices[0].y);
  Vector V(polygon->vertices[2].x - polygon->vertices[0].x, polygon->vertices[2].y - polygon->vertices[0].y);
  //CountAverageLineWidth(w);

  float _x, _y, u0, v0, u1, v1, z0, z1, z, dz, brightness0, brightness1, brightness, dbrightness;
  // start of line (A part of polygon may be out of screen. So the start and end is not always on triangle outline.)
  _x = (float)x - polygon->vertices[0].x; // a point of reference is vertices[0].
  _y = (float)y - polygon->vertices[0].y;
  u0 = (_x*V.y - _y*V.x)/(U.x*V.y - U.y*V.x);
  v0 = (-_x*U.y + _y*U.x)/(U.x*V.y - U.y*V.x);
  // end of line
  _x += (float)(w - 1);
  u1 = (_x*V.y - _y*V.x)/(U.x*V.y - U.y*V.x);
  v1 = (-_x*U.y + _y*U.x)/(U.x*V.y - U.y*V.x);
  // If uv coordinate is out of between 0 and 1 by calculation error, it should be confined. Proper tolerance may change.
  const float tolerance = 1;
  if (u0 < 0 - tolerance) u0 = 0;
  if (u0 > 1 + tolerance) u0 = 1;
  if (v0 < 0 - tolerance) v0 = 0;
  if (v0 > 1 + tolerance) v0 = 1;
  if (u1 < 0 - tolerance) u1 = 0;
  if (u1 > 1 + tolerance) u1 = 1;
  if (v1 < 0 - tolerance) v1 = 0;
  if (v1 > 1 + tolerance) v1 = 1;
  // interpolation between both edge Z
  z0 = u0*(polygon->vertices[1].z - polygon->vertices[0].z) + v0*(polygon->vertices[2].z - polygon->vertices[0].z) + polygon->vertices[0].z;
  z1 = u1*(polygon->vertices[1].z - polygon->vertices[0].z) + v1*(polygon->vertices[2].z - polygon->vertices[0].z) + polygon->vertices[0].z;
  z = z0;
  dz = (z1 - z0)/float(w);
  // interpolation between both edge brightness
  if (gouraud) {
    Vector normal0, normal1;
    normal0.x = u0*(polygon->normal[1].x - polygon->normal[0].x) + v0*(polygon->normal[2].x - polygon->normal[0].x) + polygon->normal[0].x;
    normal0.y = u0*(polygon->normal[1].y - polygon->normal[0].y) + v0*(polygon->normal[2].y - polygon->normal[0].y) + polygon->normal[0].y;
    normal0.z = u0*(polygon->normal[1].z - polygon->normal[0].z) + v0*(polygon->normal[2].z - polygon->normal[0].z) + polygon->normal[0].z;
    Affine::Normalize(&normal0);
    normal1.x = u1*(polygon->normal[1].x - polygon->normal[0].x) + v1*(polygon->normal[2].x - polygon->normal[0].x) + polygon->normal[0].x;
    normal1.y = u1*(polygon->normal[1].y - polygon->normal[0].y) + v1*(polygon->normal[2].y - polygon->normal[0].y) + polygon->normal[0].y;
    normal1.z = u1*(polygon->normal[1].z - polygon->normal[0].z) + v1*(polygon->normal[2].z - polygon->normal[0].z) + polygon->normal[0].z;
    Affine::Normalize(&normal1);
    brightness0 = -Affine::Dot(&light, &normal0);
    brightness1 = -Affine::Dot(&light, &normal1);
    brightness = brightness0;
    dbrightness = (brightness1 - brightness0)/float(w);
  }
  // interpolation between both edge texture_uv
  Vector texture_uv0, texture_uv1, texture_uv, dtexture_uv;
  if (texture){
    texture_uv0.x = u0*(polygon->texture_uv[1].x - polygon->texture_uv[0].x) + v0*(polygon->texture_uv[2].x - polygon->texture_uv[0].x) + polygon->texture_uv[0].x;
    texture_uv0.y = u0*(polygon->texture_uv[1].y - polygon->texture_uv[0].y) + v0*(polygon->texture_uv[2].y - polygon->texture_uv[0].y) + polygon->texture_uv[0].y;
    texture_uv1.x = u1*(polygon->texture_uv[1].x - polygon->texture_uv[0].x) + v1*(polygon->texture_uv[2].x - polygon->texture_uv[0].x) + polygon->texture_uv[0].x;
    texture_uv1.y = u1*(polygon->texture_uv[1].y - polygon->texture_uv[0].y) + v1*(polygon->texture_uv[2].y - polygon->texture_uv[0].y) + polygon->texture_uv[0].y;
    texture_uv = texture_uv0;
    dtexture_uv.x = (texture_uv1.x - texture_uv0.x)/float(w);
    dtexture_uv.y = (texture_uv1.y - texture_uv0.y)/float(w);
  }
  for (int i = 0; i < w; ++i) {
    if (i != 0) z += dz;
    if (z > zBuffer[y][x + i]/10.0) {
      uint16_t color;
      bool alpha = false;
      if (!gouraud) color = plainColor;
      if (gouraud && i != 0) brightness += dbrightness;
      if (gouraud && !texture) color = DecayBrightness(material.color, brightness);
      else if (gouraud && texture){
        if (i != 0) {
          texture_uv.x += dtexture_uv.x;
          texture_uv.y += dtexture_uv.y;
        }
        uint16_t tex_color;
        int tex_index = biWidth*biHeight - (uint16_t)(biHeight*texture_uv.y)*biWidth - (uint16_t)(biWidth*(1 - texture_uv.x));
        if (tex_index < 0) tex_index = 0;
        if (tex_index > biWidth*biHeight) tex_index = biWidth*biHeight;
        file = file_start + header_offset + sizeof(tex_color)*tex_index;
        file_read((uint8_t*)&tex_color, sizeof(tex_color));
        if ((tex_color & 0x8000) == 0) alpha = true;
        tex_color = ((tex_color & 0x7FE0) << 1) | (tex_color & 0x001F);
        color = DecayBrightness(tex_color, brightness);
      }
      if (!alpha) {
        backBuffer[y][x + i] = color;
        zBuffer[y][x + i] = (int16_t)(z*10.0);
      }
    }
  }
}

void PixelShader::SetMaterial(Material* material) {
  this->material.color = material->color;
  for (int i = 0; i < 21; ++i) this->material.texture_file_name[i] = material->texture_file_name[i];
  if (this->material.texture_file_name[0] == 0) textureFileAvailable = false;
  else {
    textureFileAvailable = true;
    String filename = String((char*)this->material.texture_file_name);
    for (int i = 0; i < numTexture; ++i) if (filename == textureFilename[i]) file_start = textureFile[i];  
    file = file_start;
    file = file_start + 10;
    file_read((uint8_t*)&header_offset, sizeof(header_offset));
    file = file_start + 18;
    file_read((uint8_t*)&biWidth, sizeof(biWidth));
    file_read((uint8_t*)&biHeight, sizeof(biHeight));
  }
}
