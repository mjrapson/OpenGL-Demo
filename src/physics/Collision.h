// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Mark Rapson

#pragma once

class Box;
class Ray;
class Sphere;

namespace collision
{
bool intersects(const Ray& ray, const Box& box, float& distance);
bool intersects(const Sphere& sphere, const Box& box);
// bool intersects(float x, float y, const Rectangle& rect);
// bool intersects(const Rectangle& rect1, const Rectangle& rect2);
// bool intersects(const Rectangle& rect, const Sphere& sphere);
}
