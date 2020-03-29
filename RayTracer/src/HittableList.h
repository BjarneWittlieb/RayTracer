#pragma once

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.h"
#include <memory>
#include <vector>
#include <mutex>

using std::shared_ptr;
using std::make_shared;

class HittableList
	: public Hittable
{
private:
	std::mutex * mtx;

public:
	std::vector<shared_ptr<Hittable>> Objects;

public:
	HittableList() {
		mtx = new std::mutex();
	}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void Clear() { Objects.clear(); }
	void add(shared_ptr<Hittable> object) { Objects.push_back(object); }

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const;
};

bool HittableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
	std::lock_guard<std::mutex> lock(*mtx);
	bool hitAnything = false;
	double closestUntilNow = tMax;

	for (const auto& object : Objects)
	{
		HitRecord tmpRec;
		if (object->Hit(r, tMin, tMax, tmpRec))
		{
			hitAnything = true;
			// closestUntilNow = tmpRec.t;
			// rec = tmpRec;
			if (closestUntilNow > tmpRec.t)
			{
				closestUntilNow = tmpRec.t;
				rec = tmpRec;
			}
		}
	}

	return hitAnything;
}

#endif