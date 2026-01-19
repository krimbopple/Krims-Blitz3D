
#include "std.h"
#include "meshutil.h"

MeshModel *MeshUtil::createCube( const Brush &b ){
	static Vector norms[]={
		Vector(0,0,-1),Vector(1,0,0),Vector(0,0,1),
		Vector(-1,0,0),Vector(0,1,0),Vector(0,-1,0)
	};
	static Vector tex_coords[]={
		Vector(0,0,1),Vector(1,0,1),Vector(1,1,1),Vector(0,1,1)
	};
	static int verts[]={
		2,3,1,0,3,7,5,1,7,6,4,5,6,2,0,4,6,7,3,2,0,1,5,4
	};
	static Box box( Vector(-1,-1,-1),Vector(1,1,1) );

	MeshModel *m=d_new MeshModel();
	Surface *s=m->createSurface( b );
	Surface::Vertex v;
	Surface::Triangle t;
	for( int k=0;k<24;k+=4 ){
		const Vector &normal=norms[k/4];
		for( int j=0;j<4;++j ){
			v.coords=box.corner( verts[k+j] );
			v.normal=normal;
			v.tex_coords[0][0]=v.tex_coords[1][0]=tex_coords[j].x;
			v.tex_coords[0][1]=v.tex_coords[1][1]=tex_coords[j].y;
			s->addVertex( v );
		}
		t.verts[0]=k;t.verts[1]=k+1;t.verts[2]=k+2;s->addTriangle(t);
		t.verts[1]=k+2;t.verts[2]=k+3;s->addTriangle(t);
	}
	return m;
}

MeshModel *MeshUtil::createSphere( const Brush &b,int segs ){

	int h_segs=segs*2,v_segs=segs;

	MeshModel *m=d_new MeshModel();
	Surface *s=m->createSurface( b );

	Surface::Vertex v;
	Surface::Triangle t;

	v.coords=v.normal=Vector(0,1,0);
	int k;
	for( k=0;k<h_segs;++k ){
		v.tex_coords[0][0]=v.tex_coords[1][0]=(k+.5f)/h_segs;
		v.tex_coords[0][1]=v.tex_coords[1][1]=0;
		s->addVertex( v );
	}
	for( k=1;k<v_segs;++k ){
		float pitch=k*PI/v_segs-HALFPI;
		for( int j=0;j<=h_segs;++j ){
			float yaw=(j%h_segs)*TWOPI/h_segs;
			v.coords=v.normal=rotationMatrix( pitch,yaw,0 ).k;
			v.tex_coords[0][0]=v.tex_coords[1][0]=float(j)/float(h_segs);
			v.tex_coords[0][1]=v.tex_coords[1][1]=float(k)/float(v_segs);
			s->addVertex( v );
		}
	}
	v.coords=v.normal=Vector(0,-1,0);
	for( k=0;k<h_segs;++k ){
		v.tex_coords[0][0]=v.tex_coords[1][0]=(k+.5f)/h_segs;
		v.tex_coords[0][1]=v.tex_coords[1][1]=1;
		s->addVertex( v );
	}
	for( k=0;k<h_segs;++k ){
		t.verts[0]=k;
		t.verts[1]=t.verts[0]+h_segs+1;
		t.verts[2]=t.verts[1]-1;
		s->addTriangle( t );
	}
	for( k=1;k<v_segs-1;++k ){
		for( int j=0;j<h_segs;++j ){
			t.verts[0]=k*(h_segs+1)+j-1;
			t.verts[1]=t.verts[0]+1;
			t.verts[2]=t.verts[1]+h_segs+1;
			s->addTriangle( t );
			t.verts[1]=t.verts[2];
			t.verts[2]=t.verts[1]-1;
			s->addTriangle( t );
		}
	}
	for( k=0;k<h_segs;++k ){
		t.verts[0]=(h_segs+1)*(v_segs-1)+k-1;
		t.verts[1]=t.verts[0]+1;
		t.verts[2]=t.verts[1]+h_segs;
		s->addTriangle( t );
	}

	return m;
}

MeshModel *MeshUtil::createCylinder( const Brush &b,int segs,bool solid ){

	MeshModel *m=d_new MeshModel();
	Surface::Vertex v;
	Surface::Triangle t;

	Surface *s=m->createSurface( b );
	int k;
	for( k=0;k<=segs;++k ){
		float yaw=(k%segs)*TWOPI/segs;
		v.coords=rotationMatrix( 0,yaw,0 ).k;
		v.coords.y=1;
		v.normal=Vector(v.coords.x,0,v.coords.z);
		v.tex_coords[0][0]=v.tex_coords[1][0]=float(k)/segs;
		v.tex_coords[0][1]=v.tex_coords[1][1]=0;
		s->addVertex( v );
		v.coords.y=-1;
		v.tex_coords[0][0]=v.tex_coords[1][0]=float(k)/segs;
		v.tex_coords[0][1]=v.tex_coords[1][1]=1;
		s->addVertex( v );
	}
	for( k=0;k<segs;++k ){
		t.verts[0]=k*2;
		t.verts[1]=t.verts[0]+2;
		t.verts[2]=t.verts[1]+1;
		s->addTriangle( t );
		t.verts[1]=t.verts[2];
		t.verts[2]=t.verts[1]-2;
		s->addTriangle( t );
	}

	if( !solid ) return m;

	s=m->createSurface( b );

	for( k=0;k<segs;++k ){
		float yaw=k*TWOPI/segs;
		v.coords=rotationMatrix( 0,yaw,0 ).k;
		v.coords.y=1;v.normal=Vector(0,1,0);
		v.tex_coords[0][0]=v.tex_coords[1][0]=v.coords.x*.5f+.5f;
		v.tex_coords[0][1]=v.tex_coords[1][1]=v.coords.z*.5f+.5f;
		s->addVertex(v);
		v.coords.y=-1;v.normal=Vector( 0,-1,0 );
		s->addVertex(v);
	}
	for( k=2;k<segs;++k ){
		t.verts[0]=0;
		t.verts[1]=k*2;
		t.verts[2]=(k-1)*2;
		s->addTriangle( t );
		t.verts[0]=1;
		t.verts[1]=(k-1)*2+1;
		t.verts[2]=k*2+1;
		s->addTriangle( t );
	}

	return m;
}

MeshModel *MeshUtil::createCone( const Brush &b,int segs,bool solid ){
	MeshModel *m=d_new MeshModel();
	Surface::Vertex v;
	Surface::Triangle t;

	Surface *s;
	s=m->createSurface( b );
	int k;
	v.coords=v.normal=Vector(0,1,0);
	for( k=0;k<segs;++k ){
		v.tex_coords[0][0]=v.tex_coords[1][0]=(k+.5f)/segs;
		v.tex_coords[0][1]=v.tex_coords[1][1]=0;
		s->addVertex( v );
	}
	for( k=0;k<=segs;++k ){
		float yaw=(k%segs)*TWOPI/segs;
		v.coords=yawMatrix( yaw ).k;v.coords.y=-1;
		v.normal=Vector( v.coords.x,0,v.coords.z );
		v.tex_coords[0][0]=v.tex_coords[1][0]=float(k)/segs;
		v.tex_coords[0][1]=v.tex_coords[1][1]=1;
		s->addVertex( v );
	}
	for( k=0;k<segs;++k ){
		t.verts[0]=k;
		t.verts[1]=k+segs+1;
		t.verts[2]=k+segs;
		s->addTriangle( t );
	}
	if( !solid ) return m;
	s=m->createSurface( b );
	for( k=0;k<segs;++k ){
		float yaw=k*TWOPI/segs;
		v.coords=yawMatrix( yaw ).k;v.coords.y=-1;
		v.normal=Vector( v.coords.x,0,v.coords.z );
		v.tex_coords[0][0]=v.tex_coords[1][0]=v.coords.x*.5f+.5f;
		v.tex_coords[0][1]=v.tex_coords[1][1]=v.coords.z*.5f+.5f;
		s->addVertex( v );
	}
	t.verts[0]=0;
	for( k=2;k<segs;++k ){
		t.verts[1]=k-1;
		t.verts[2]=k;
		s->addTriangle( t );
	}
	return m;
}

MeshModel* MeshUtil::createCapsule(const Brush& b, int segs, float height, float radius) {

	float half_height = height * 0.5f;
	float cylinder_height = height - 2.0f * radius;

	if (cylinder_height < 0) {
		cylinder_height = 0;
	}

	float half_cylinder = cylinder_height * 0.5f;

	int ring_segments = segs;
	int vertical_segments = segs / 2;

	MeshModel* capsule = d_new MeshModel();
	Surface* surf = capsule->createSurface(b);

	std::vector<Vector> vertices;
	std::vector<Vector2> tex_coords;

	for (int ring = 0; ring <= ring_segments; ring++) {
		float theta = (float(ring) / ring_segments) * TWOPI;
		float cos_theta = cosf(theta);
		float sin_theta = sinf(theta);

		// bottom hemisphere vertices
		for (int vert = 0; vert <= vertical_segments; vert++) {
			float phi = HALFPI * (float(vert) / vertical_segments);
			float cos_phi = cosf(phi);
			float sin_phi = sinf(phi);

			float x = radius * cos_theta * sin_phi;
			float y = -half_cylinder - radius * cos_phi;
			float z = radius * sin_theta * sin_phi;

			vertices.push_back(Vector(x, y, z));
			tex_coords.push_back(Vector2(
				float(ring) / ring_segments,
				float(vert) / (vertical_segments * 2)
			));
		}

		// cylinder vertices (if any)
		if (cylinder_height > 0) {
			for (int vert = 1; vert < vertical_segments; vert++) {
				float t = float(vert) / vertical_segments;
				float y = -half_cylinder + cylinder_height * t;

				float x = radius * cos_theta;
				float z = radius * sin_theta;

				vertices.push_back(Vector(x, y, z));
				tex_coords.push_back(Vector2(
					float(ring) / ring_segments,
					0.25f + 0.5f * t
				));
			}
		}

		// top hemisphere vertices
		for (int vert = 0; vert <= vertical_segments; vert++) {
			float phi = HALFPI * (1.0f - float(vert) / vertical_segments);
			float cos_phi = cosf(phi);
			float sin_phi = sinf(phi);

			float x = radius * cos_theta * sin_phi;
			float y = half_cylinder + radius * cos_phi;
			float z = radius * sin_theta * sin_phi;

			vertices.push_back(Vector(x, y, z));
			tex_coords.push_back(Vector2(
				float(ring) / ring_segments,
				0.5f + float(vert) / (vertical_segments * 2)
			));
		}
	}

	for (size_t i = 0; i < vertices.size(); i++) {
		Surface::Vertex v;
		v.coords = vertices[i];
		v.color = 0xffffffff;
		v.tex_coords[0][0] = tex_coords[i].x;
		v.tex_coords[0][1] = tex_coords[i].y;
		v.normal = vertices[i].normalized();

		if (i >= (ring_segments + 1) * (vertical_segments + 1) &&
			i < vertices.size() - (ring_segments + 1) * (vertical_segments + 1)) {
			int local_idx = i - (ring_segments + 1) * (vertical_segments + 1);
			int ring_idx = local_idx % ((vertical_segments - 1) + 2 * (vertical_segments + 1));

			if (ring_idx < (vertical_segments - 1)) {
				float theta = (float(ring_idx / (vertical_segments - 1)) / ring_segments) * TWOPI;
				v.normal = Vector(cosf(theta), 0, sinf(theta));
			}
		}

		surf->addVertex(v);
	}

	int vertices_per_slice = (vertical_segments + 1) + (cylinder_height > 0 ? (vertical_segments - 1) : 0) + (vertical_segments + 1);

	for (int ring = 0; ring < ring_segments; ring++) {
		for (int slice = 0; slice < vertices_per_slice - 1; slice++) {
			int v0 = ring * vertices_per_slice + slice;
			int v1 = (ring + 1) * vertices_per_slice + slice;
			int v2 = ring * vertices_per_slice + slice + 1;
			int v3 = (ring + 1) * vertices_per_slice + slice + 1;

			Surface::Triangle t1;
			t1.verts[0] = v0;
			t1.verts[1] = v1;
			t1.verts[2] = v2;
			surf->addTriangle(t1);

			Surface::Triangle t2;
			t2.verts[0] = v1;
			t2.verts[1] = v3;
			t2.verts[2] = v2;
			surf->addTriangle(t2);
		}
	}

	capsule->flipTriangles();
	capsule->updateNormals();

	return capsule;
}

void MeshUtil::lightMesh( MeshModel *m,const Vector &pos,const Vector &rgb,float range ){
	if( range ){
		float att=1.0f/range;
		const MeshModel::SurfaceList &surfs=m->getSurfaces();
		for( int k=0;k<surfs.size();++k ){
			Surface *s=surfs[k];
			for( int j=0;j<s->numVertices();++j ){
				const Surface::Vertex &v=s->getVertex( j );
				Vector lv=pos-v.coords;
				float dp=v.normal.normalized().dot( lv );
				if( dp<=0 ) continue;
				float d=lv.length();
				float i=1/(d*att)*(dp/d);
				s->setColor( j,s->getColor(j)+rgb*i );
			}
		}
	}else{
		const MeshModel::SurfaceList &surfs=m->getSurfaces();
		for( int k=0;k<surfs.size();++k ){
			Surface *s=surfs[k];
			for( int j=0;j<s->numVertices();++j ){
				const Surface::Vertex &v=s->getVertex( j );
				s->setColor( j,s->getColor(j)+rgb );
			}
		}
	}
}
