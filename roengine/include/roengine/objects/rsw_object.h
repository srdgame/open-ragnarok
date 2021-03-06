/* $Id$ */
#ifndef __RSW_OBJECT_H
#define __RSW_OBJECT_H

#include "../gl_object.h"
#include "ro/types/rsw.h"
#include "ro/types/gat.h"
#include "ro/types/gnd.h"
#include "../cache_manager.h"
#include "../texturecache.h"
#include "rsm_object.h"

/**
 * Stores the data of the map and draws it as requested.
 */
class RswObject : public GLObject {
private:
	sdle::Texture m_grid;
protected:
	/** The size of the tile (default: 10.0f) */
	static float m_tilesize;

	const ro::RSW* rsw;
	const ro::GND* gnd;
	const ro::GAT* gat;

	/** Textures used by the ground */
	TextureCache textures;
	TextureCache lighmapTextures;
	std::map<unsigned short, unsigned int*> lightmap_texmap; // TODO

	/** Water textures */
	TextureCache water_tex;

	/** Draws a GND surface */
	void DrawSurface(const ro::GND::Surface& surface, const float* vertices);

	/** Draws the Ground */
	void DrawGND();

	/** Draws the water */
	void DrawWater();

	/** Draws the objects of the map */
	void DrawObjects();

	unsigned int gnd_gl;

	/** The current water frame to draw */
	unsigned int m_waterframe;

	/** The ticks passed since last frame changed */
	unsigned int m_waterdelay;

	/** Wave offset in degrees [-180, 180[ */
	float m_waveOffset;

	float world_x;
	float world_y;
	float world_z;

	int mouse_x, mouse_y;

	static void getRot(float sizex, float sizey, float rot[16]);

	CacheManager& m_cache;

public:
	RswObject(const ro::RSW*, CacheManager&);
	virtual ~RswObject();

	/**
	 * Loads all textures needed by the map
	 */
	bool loadTextures(CacheManager&);

	virtual void Draw();

	void setMouse(int screen_x, int screen_y);

	void DrawSelection(int mapx, int mapy) const;
	virtual bool isInFrustum(const Frustum&) const;

	/**
	 * Retrieves the GL position (rx, ry, rz) of the given map coordinates.
	 *
	 * If the given positions are not integer, it will return a medium of the adjacent positions.
	 *
	 * @param mapx map X coordinate
	 * @param mapy map Y coordinate
	 * @param rx float world X coordinate
	 * @param ry float world Z coordinate
	 * @param ry float world Z coordinate
	 */
	void getWorldPosition(float mapx, float mapy, float *rx, float *ry, float *rz);

	/**
	 * Retrieves the RSW file used to draw this map
	 */
	const ro::RSW* getRSW() const;

	/**
	 * Retrieves the GND file used to draw this map
	 */
	const ro::GND* getGND() const;

	bool valid() const;

	/**
	 * Reads a file from the cache and returns a new RswObject pointer to it.
	 *
	 * @param cache CacheManager to read stuff from
	 * @param map the map name to load (without extension)
	 * @return a New RswObject pointer on success or NULL on failure
	 */
	static RswObject* open(CacheManager&, const char* map);

	int getMouseMapX() const;
	int getMouseMapY() const;

	float getWorldX() const;
	float getWorldY() const;
	float getWorldZ() const;
};

#endif /* __RSW_OBJECT_H */
