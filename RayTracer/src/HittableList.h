#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.h"
#include <memory>
#include <vector>
#include <iostream>

using std::shared_ptr;
using std::make_shared;

class HitableList
	: public Hitable
{
public:
	std::vector<shared_ptr<Hitable>> Objects;

public:
	HitableList() {
	}
	HitableList(shared_ptr<Hitable> object) {
		Add(object);
	}

	void Clear() { Objects.clear(); }
	void Add(shared_ptr<Hitable> object) { Objects.push_back(object); }

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const;
};

bool HitableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
	bool hitAnything = false;
	double closestUntilNow = tMax;

	for (const auto& object : Objects)
	{
		HitRecord tmpRec;
		if (object->Hit(r, tMin, tMax, tmpRec))
		{
			hitAnything = true;
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