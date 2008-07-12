/* $id$ */
#ifndef __RSW_H
#define __RSW_H

#include "object.h"

namespace RO {
	/**
	 * Interface for RSW format objects/files
	 */
	class MYLIB_DLLAPI RSW : public Object {
#pragma pack(push)
#pragma pack(1)
		/**
		 * Base class for RSW objects
		 */
		class Object {
		public:
			typedef enum {
				OT_Unknown,
				OT_Model,
				OT_Light,
				OT_Sound,
				OT_Effect
			} ObjectType;

			virtual bool readStream(std::istream&) = 0;
			virtual bool writeStream(std::ostream&) const = 0;

			Object(ObjectType);
			virtual ~Object();

			/** Returns the object type */
			ObjectType getType() const;

			virtual void Dump(std::ostream& = std::cout) const;
			
			/** Check if we are the same type as the parameter */
			bool isType(ObjectType) const;

		protected:
			ObjectType m_type;

			unsigned int datasize;
		};

		/**
		 * RSW Object Model
		 */
		class Model : public Object {
		public:
			Model();
			virtual ~Model();
			virtual bool readStream(std::istream&);
			virtual bool writeStream(std::ostream&) const;

			virtual void Dump(std::ostream& = std::cout) const;

			typedef struct {
				char m_name[40];
				int unk1;// (version >= 1.3)
				float unk2;// (version >= 1.3)
				float unk3;// (version >= 1.3)
				char filename[40];
				char reserved[40];
				char type[20];
				char sound[20];
				char todo1[40];
				float pos[3];
				float rot[3];
				float scale[3];
			} ModelData;

			const ModelData *data;

		protected:
			ModelData m_data;
		};

		/**
		 * RSW Object Light
		 */
		class Light : public Object {
		public:
			Light();
			virtual ~Light();
			virtual bool readStream(std::istream&);
			virtual bool writeStream(std::ostream&) const;

			typedef struct {
				char name[40];
				float pos[3];
				char unk1[40];
				float color[3];
				float unk2;
			} LightData;

			const LightData* data;
		protected:
			LightData m_data;
		};

		/**
		 * RSW Object Sound
		 */
		class Sound : public Object {
		public:
			Sound();
			virtual ~Sound();
			virtual bool readStream(std::istream&);
			virtual bool writeStream(std::ostream&) const;

			typedef struct {
				char name[80];
				char filename[80];
				float unk[8];
			} SoundData;

			const SoundData *data;
		public:
			SoundData m_data;
		};

		/**
		 * RSW Object Effect
		 */
		class Effect : public Object {
		public:
			Effect();
			virtual ~Effect();
			virtual bool readStream(std::istream&);
			virtual bool writeStream(std::ostream&) const;

			typedef struct {
				char name[40];
				float unk1[9];
				int category;
				float pos[3];
				int type;
				float loop;
				float unk2[2];
				int unk3[2];
			} EffectData;

			const EffectData *data;

		protected:
			EffectData m_data;
		};
#pragma pack(pop)
	protected:
		Object** m_objects;

	public:
		char ini_file[40];
		char gnd_file[40];
		char gat_file[40];
		char scr_file[40];

		// Water
		struct strWater {
			float height;
			unsigned int type;
			float amplitude;
			float phase;
			float surface_curve_level;
			int texture_cycling;
		} water;
		// Light
		struct strLight {
			float ambient[3];
			float diffuse[3];
			float shadow[3];
			float alpha;
		} light;

		int unk[3];
		unsigned int object_count;

		RSW();
		virtual ~RSW();

		virtual bool readStream(std::istream&);
		virtual bool writeStream(std::ostream&);
		virtual void Dump(std::ostream& = std::cout) const;
		void Clear();
	};
}

#endif /* __RSW_H */
