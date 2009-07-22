#include "stdafx.h"

#include "roengine/objects/act_object.h"

#include <GL/gl.h>
#include <GL/glu.h>

float act_angles[] = {
	-157.5,
	-112.5,
	-67.5,
	-22.5,
	22.5,
	112.5,
	157.5
};

ActObject::ActObject() : GLObject(), FullAct() {
	curframe = 0;
	curaction = 0;
}

ActObject::ActObject(const ActObject& o) : FullAct(o) {
	curframe = 0;
	curaction = o.curaction;
}

ActObject::ActObject(const RO::ACT* a, const rogl::Texture::PointerCache& t) : FullAct(a, t) {
	curframe = 0;
	curaction = 0;
}

ActObject::~ActObject() {
}

void ActObject::Billboard() {
	// Cheat Cylindrical. Credits: http://www.lighthouse3d.com/opengl/billboarding/index.php3?billCheat1
	float modelview[16];
	int i,j;

	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	float v;

	for( i=0; i<3; i+=2 ) 
	    for( j=0; j<3; j++ ) {
			if ( i==j ) {
				if (i == 0)
					v = scalex;
				else if (i == 1)
					v = scaley;
				else
					v = 1.0f;
				modelview[i*4+j] = v;
			}
			else {
				modelview[i*4+j] = 0.0;
			}
	    }

	glLoadMatrixf(modelview);
}

void ActObject::Window(float x, float y, const rogl::Texture::Pointer& tex, bool mirrorX, bool mirrorY) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.90f);

	tex.Activate();

	float w = (float)tex->getWidth();
	float h = (float)tex->getHeight();

	float u, u1;
	float v, v1;

	if (!mirrorY) {
		v1 = 0.0f;
		v = tex->getMaxV();
	}
	else {
		v = 0.0f;
		v1 = tex->getMaxV();
	}

	if (!mirrorX) {
		u1 = 0.0f;
		u = tex->getMaxU();
	}
	else {
		u = 0.0f;
		u1 = tex->getMaxU();
	}

	glBegin(GL_QUADS);
	glTexCoord2f(u1, v);
	glVertex3f(x - w/2, y,     0);
	glTexCoord2f(u1, v1);
	glVertex3f(x - w/2, y + h, 0);
	glTexCoord2f(u, v1);
	glVertex3f(x + w/2, y + h, 0);
	glTexCoord2f(u, v);
	glVertex3f(x + w/2, y,     0);
	glEnd();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
}

void ActObject::DrawAct(const RO::ACT::Pat& pat, rogl::Texture::Pointer& t) {
	Billboard();
	Window((float)pat.spr[0].x, (float)pat.spr[0].y, t, (pat.spr[0].mirrorOn == 1));
}

void ActObject::Draw() {
	if (m_act == NULL)
		return;
	int xact = curaction;

	float angle;
	angle = getAngle(Vector3f::UNIT_Z);
	angle = angle * 180.0f / 3.1415f;

	// Dumb check. Need to improve this.
	int offset = 4;

	if (angle < act_angles[0]) {
		offset = 4;
	}
	else if (angle < act_angles[1]) {
		offset = 3;
	}
	else if (angle < act_angles[2]) {
		offset = 2;
	}
	else if (angle < act_angles[3]) {
		offset = 1;
	}
	else if (angle < act_angles[4]) {
		offset = 0;
	}
	else if (angle < act_angles[5]) {
		offset = 7;
	}
	else if (angle < act_angles[6]) {
		offset = 6;
	}
	else if (angle < act_angles[7]) {
		offset = 5;
	}
	else {
		offset = 4;
	}

	xact += offset;
	// std::cout << "\t\t" << angle << " (" << offset << ")\r";

	const RO::ACT::Act& cur_act = m_act->getAct(xact);
	int texidx = 0;
	bool mirror;

	m_time += m_tickdelay;
	while (m_time > 100) {
		m_time -= 100;
		curframe++;
		if (curframe >= cur_act.patnum)
			curframe = 0;
	}

	const RO::ACT::Pat& cpat = cur_act[curframe];

	texidx = cpat.spr[0].sprNo;
	mirror = (cpat.spr[0].mirrorOn == 1)?true:false;

	glTranslatef(pos[0], pos[1], pos[2]);
	DrawAct(cpat, m_textures[texidx]);
}
