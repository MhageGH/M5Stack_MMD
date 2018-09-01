#include "PixelShader.h"

PixelShader::PixelShader(Vector light0) : Height(240), Width(320), _height(180), _width(240), minZ(-32768), txHeight(128), txWidth(128) {
  backBuffer = (uint16_t*)ps_malloc(sizeof(uint16_t)*_height*_width);
  zBuffer = (int16_t*)ps_malloc(sizeof(int16_t)*_height*_width);
  txBuffer = (uint16_t*)ps_malloc(sizeof(uint16_t*)*txHeight*txWidth);
  for (int i = 0; i < _height*_width; ++i) backBuffer[i] = 0;
  for (int i = 0; i < _height*_width; ++i) zBuffer[i] = minZ;
  light = light0;
}

PixelShader::~PixelShader() {
  free(backBuffer);
  free(zBuffer);
  free(txBuffer);
}

void PixelShader::Flip() {
  M5.Lcd.drawBitmap((Width - _width)/2, (Height - _height)/2, _width, _height, (uint8_t*)backBuffer);
}

void PixelShader::Clear(uint16_t color) {
 for (int i = 0; i < _height*_width; ++i) backBuffer[i] = color;
 for (int i = 0; i < _height*_width; ++i) zBuffer[i] = minZ;
}

uint16_t PixelShader::DecayBrightness(uint16_t color, float rate){
  rate = abs(rate);
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

bool PixelShader::Culling(Polygon* polygon) {
    float P1x = polygon->vertices[1].x - polygon->vertices[0].x;
    float P1y = polygon->vertices[1].y - polygon->vertices[0].y;
    float P2x = polygon->vertices[2].x - polygon->vertices[0].x;
    float P2y = polygon->vertices[2].y - polygon->vertices[0].y;
    float OPz = P1x*P2y - P1y*P2x;
    return OPz < 0;
}

void PixelShader::DrawPolygon(Polygon* polygon) {
  if(material->useCulling && Culling(polygon)) return;
  Point p[3];
  for (int i = 0; i < 3; ++i) p[i] = polygon->vertices[i].ToPoint();
  int32_t a, b, y, last;
  if (p[0].Y > p[1].Y) {swap_coord(p[0].Y, p[1].Y); swap_coord(p[0].X, p[1].X);}
  if (p[1].Y > p[2].Y) {swap_coord(p[2].Y, p[1].Y); swap_coord(p[2].X, p[1].X);}
  if (p[0].Y > p[1].Y) {swap_coord(p[0].Y, p[1].Y); swap_coord(p[0].X, p[1].X);}
  if (p[0].Y == p[2].Y) { // Handle awkward all-on-same-line case as its own thing
    a = b = p[0].X;
    if (p[1].X < a)      a = p[1].X;
    else if (p[1].X > b) b = p[1].X;
    if (p[2].X < a)      a = p[2].X;
    else if (p[2].X > b) b = p[2].X;
    DrawHLine(a, p[0].Y, b - a + 1, polygon);
    return;
  }
  int32_t dx01 = p[1].X - p[0].X, dy01 = p[1].Y - p[0].Y, dx02 = p[2].X - p[0].X, dy02 = p[2].Y - p[0].Y, dx12 = p[2].X - p[1].X, dy12 = p[2].Y - p[1].Y,sa = 0, sb = 0;
  if (p[1].Y == p[2].Y) last = p[1].Y;  // Include p[1].Y scanline
  else         last = p[1].Y - 1; // Skip it
  for (y = p[0].Y; y <= last; y++) {
    a   = p[0].X + sa / dy01;
    b   = p[0].X + sb / dy02;
    sa += dx01;
    sb += dx02;
    if (a > b) swap_coord(a, b);
    DrawHLine(a, y, b - a + 1, polygon);
  }
  sa = dx12 * (y - p[1].Y);
  sb = dx02 * (y - p[0].Y);
  for (; y <= p[2].Y; y++) {
    a   = p[1].X + sa / dy12;
    b   = p[0].X + sb / dy02;
    sa += dx12;
    sb += dx02;
    if (a > b) swap_coord(a, b);
    DrawHLine(a, y, b - a + 1, polygon);
  }
}

void PixelShader::DrawHLine(int32_t x, int32_t y, int32_t w, Polygon* polygon) {
  float inv_w = 1.0f/float(w);
  if (x < 0) {
    w += x;
    x = 0;
  }
  if ((x >= _width) || (y >= _height) || (y < 0) || (w < 1)) return;
  if ((x + w - 1) >= _width)  w = _width - x;
  Vector U(polygon->vertices[1].x - polygon->vertices[0].x, polygon->vertices[1].y - polygon->vertices[0].y);
  Vector V(polygon->vertices[2].x - polygon->vertices[0].x, polygon->vertices[2].y - polygon->vertices[0].y);

  float _x, _y, u0, v0, u1, v1, z0, z1, z, dz, brightness0, brightness1, brightness, dbrightness;

  // start of line. it is not always on outline of polygon because it is possible that a part of polygon is out of screen.
  _x = (float)x - polygon->vertices[0].x; // vertices[0] is base when uv calculation
  _y = (float)y - polygon->vertices[0].y;
  float iuv = 1.0f/(U.x*V.y - U.y*V.x);
  u0 = (_x*V.y - _y*V.x)*iuv;
  v0 = (-_x*U.y + _y*U.x)*iuv;

  // end of line
  _x += (float)(w - 1);
  u1 = (_x*V.y - _y*V.x)*iuv;
  v1 = (-_x*U.y + _y*U.x)*iuv;

  // confine between 0 and 1 to prevent divergence. the proper tolerance may change.
  const float tolerance = 1;
  if (u0 < 0 - tolerance) u0 = 0;
  if (u0 > 1 + tolerance) u0 = 1;
  if (v0 < 0 - tolerance) v0 = 0;
  if (v0 > 1 + tolerance) v0 = 1;
  if (u1 < 0 - tolerance) u1 = 0;
  if (u1 > 1 + tolerance) u1 = 1;
  if (v1 < 0 - tolerance) v1 = 0;
  if (v1 > 1 + tolerance) v1 = 1;
  
  // complement between edges' Z
  z0 = u0*(polygon->vertices[1].z - polygon->vertices[0].z) + v0*(polygon->vertices[2].z - polygon->vertices[0].z) + polygon->vertices[0].z;
  z1 = u1*(polygon->vertices[1].z - polygon->vertices[0].z) + v1*(polygon->vertices[2].z - polygon->vertices[0].z) + polygon->vertices[0].z;
  z = z0;
  dz = (z1 - z0)*inv_w;
  
  // complement between edges' brightness
  Vector normal0, normal1;
  normal0.x = u0*(polygon->normal[1].x - polygon->normal[0].x) + v0*(polygon->normal[2].x - polygon->normal[0].x) + polygon->normal[0].x;
  normal0.y = u0*(polygon->normal[1].y - polygon->normal[0].y) + v0*(polygon->normal[2].y - polygon->normal[0].y) + polygon->normal[0].y;
  normal0.z = u0*(polygon->normal[1].z - polygon->normal[0].z) + v0*(polygon->normal[2].z - polygon->normal[0].z) + polygon->normal[0].z;
  normal1.x = u1*(polygon->normal[1].x - polygon->normal[0].x) + v1*(polygon->normal[2].x - polygon->normal[0].x) + polygon->normal[0].x;
  normal1.y = u1*(polygon->normal[1].y - polygon->normal[0].y) + v1*(polygon->normal[2].y - polygon->normal[0].y) + polygon->normal[0].y;
  normal1.z = u1*(polygon->normal[1].z - polygon->normal[0].z) + v1*(polygon->normal[2].z - polygon->normal[0].z) + polygon->normal[0].z;
  brightness0 = -Affine::Dot(&light, &normal0);
  brightness1 = -Affine::Dot(&light, &normal1);
  brightness = brightness0;
  dbrightness = (brightness1 - brightness0)*inv_w;
  
  // complement between edges' texture_uv
  Vector texture_uv0, texture_uv1, texture_uv, dtexture_uv;
  if (textureEnable){
    texture_uv0.x = u0*(polygon->texture_uv[1].x - polygon->texture_uv[0].x) + v0*(polygon->texture_uv[2].x - polygon->texture_uv[0].x) + polygon->texture_uv[0].x;
    texture_uv0.y = u0*(polygon->texture_uv[1].y - polygon->texture_uv[0].y) + v0*(polygon->texture_uv[2].y - polygon->texture_uv[0].y) + polygon->texture_uv[0].y;
    texture_uv1.x = u1*(polygon->texture_uv[1].x - polygon->texture_uv[0].x) + v1*(polygon->texture_uv[2].x - polygon->texture_uv[0].x) + polygon->texture_uv[0].x;
    texture_uv1.y = u1*(polygon->texture_uv[1].y - polygon->texture_uv[0].y) + v1*(polygon->texture_uv[2].y - polygon->texture_uv[0].y) + polygon->texture_uv[0].y;
    texture_uv = texture_uv0;
    dtexture_uv.x = (texture_uv1.x - texture_uv0.x)*inv_w;
    dtexture_uv.y = (texture_uv1.y - texture_uv0.y)*inv_w;
  }

  // draw
  for (int i = 0; i < w; ++i) {
    if (i != 0) z += dz;
    if (z > zBuffer[y*_width + x + i]*0.1f) {
      uint16_t color;
      bool alpha = false;
      if (i != 0) brightness += dbrightness;
      if (!textureEnable) color = DecayBrightness(material->color, brightness);
      else if (textureEnable){
        if (i != 0) {
          texture_uv.x += dtexture_uv.x;
          texture_uv.y += dtexture_uv.y;
        }
        uint16_t tex_color;
        int tx_y = txHeight - 1 - (int)(txHeight*texture_uv.y);
        int tx_x = (int)(txWidth*texture_uv.x);
        if (tx_y < 0) tx_y = 0;
        if (tx_y > txHeight - 1) tx_y = txHeight - 1;
        if (tx_x < 0) tx_x = 0;
        if (tx_x > txWidth - 1) tx_x = txWidth - 1;
        tex_color = txBuffer[tx_y*txWidth + tx_x];
        
        if ((tex_color & 0x8000) == 0) alpha = true;
        tex_color = ((tex_color & 0x7FE0) << 1) | (tex_color & 0x001F);
        color = DecayBrightness(tex_color, brightness);
      }
      if (!alpha) {
        backBuffer[y*_width + x + i] = color;
        zBuffer[y*_width + x + i] = (int16_t)(z*10.0f);
      }
    }
  }
}

void PixelShader::SetMaterial(Material* material) {
  this->material = material;
  textureEnable = false;
}

void PixelShader::SetTexture(Texture* texture) {
  textureEnable = true;
  memcpy(txBuffer, texture->imageData, sizeof(texture->imageData));
}

