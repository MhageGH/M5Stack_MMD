//
// blit.h
//
#pragma once

#define BLIT_QUEUE_MAX 1

namespace blit
{
void Setup();
int GetIndex();
void Request(int x, int y, int w, int h, void *b, void *z, int idx);
} // namespace blit
