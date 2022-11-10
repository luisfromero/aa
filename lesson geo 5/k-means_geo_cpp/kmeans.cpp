#include "umaobd.h" 
#include <iostream> 
#include <list> 
#include <vector> 
#include <iterator> 

using namespace std;

vector <list <tripline*> > clusters;

tripline *centroide(list <tripline*> grupo)
{
	tripline *newtrack=new tripline();
	newtrack->fill(256);
    
	auto it = grupo.begin();
	for (int i = 0; i < 256; i++)
		newtrack->points[i] = new trippoint((*it)->points[i]);
	while(++it!=grupo.end())
	{
		for (int i = 0; i < 256; i++)
		{
			newtrack->points[i]->newlat += (*it)->points[i]->newlat;
			newtrack->points[i]->newlon += (*it)->points[i]->newlon;
		}
	}
	for (int i = 0; i < 256; i++) {
		newtrack->points[i]->newlat /= 5;
		newtrack->points[i]->newlon /= 5;
		newtrack->points[i]->latitude = newtrack->points[i]->newlat;
		newtrack->points[i]->longitude = newtrack->points[i]->newlon;
	}
	return newtrack;

}

void agrupa5(tripline ** tracks)
{
	list <tripline*> grupo;
	grupo.push_front(tracks[0]);
	grupo.push_front(tracks[2]);
	grupo.push_front(tracks[3]);
	grupo.push_front(tracks[4]);
	grupo.push_front(tracks[5]);
	tripline *central = centroide(grupo);
	grupo.push_front(central);
	clusters.push_back(grupo);
	//return grupo;
}
tripline * kmeans(tripline** tracks)
{
	agrupa5(tracks);
	return *clusters[0].begin();
}
