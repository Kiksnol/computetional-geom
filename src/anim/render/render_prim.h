/* FILE NAME: render_prim.h
 * PROGRAMMER: VB5
 * LAST UPDATE: 24.07.2021
 * PURPOSE: Render prim animation library
 */

#ifndef _render_prim_h_
#define _render_prim_h_

#include "../../def.h"
#include "res/materials.h"
#include "res/texture.h"

#pragma warning(disable: 6386)

namespace nicg
{
  class vertex
  {
  public:
    nicg::vec3 P;   /* position */
    nicg::vec2 T;   /* texture coordinates */
    nicg::vec3 N;   /* normal */
    nicg::vec4 C;   /* color (r,g,b,a) */
    vertex(VOID) : P(0), T(0), N(0), C(0)
    {
    }
  }; /* end of 'vertex' class */
  
  enum struct prim_type
  {
    PRIM_TRIMESH,  /* Triangle mesh - array of triangles */
    PRIM_TRISTRIP, /* Triangle strip - array of stripped triangles */
    PRIM_LINES,    /* Line segments (by 2 points) */
    PRIM_POINTS,   /* Arrauy of points */
    PRIM_PATH,     /* path of points */
  };

  class prim
  {
  public:
    prim_type Type;
    UINT
      NumOfEl,          /* Number of elements */
      VA,               /* Vertex array Id */
      VBuf,             /* Vertex buffer Id */
      IBuf;             /* Index buffer Id */
    matr Trans;   /* Additional transformation matrix */
    material *Mtl;
    vec3 MinBB;
    vec3 MaxBB;


    prim( VOID ) : NumOfEl(0), VA(0), VBuf(0), IBuf(0), Type(prim_type::PRIM_TRIMESH)
    {
      Trans.intoIdentity();
    }

    ~prim( VOID )
    {
      OutputDebugString("Prim destructed\n");
    }

    /* Primitive creation function.
     * ARGUMENTS:
     *   - prim type:
     *       prim_type Type;
     *   - vertex attributes array:
     *       nicg::vertex<FLT> *V;
     *   - number of vertices:
     *       INT NumOfV;
     *   - index array (for trimesh – by 3 ones, may be NULL)
     *       INT *I;
     *   - number of indices
     *       INT NumOfI;
     * RETURNS: None.
     */
    VOID PrimCreate( nicg::prim_type Type, nicg::vertex *V, INT NumOfV, INT *I, INT NumOfI );

    /* Free render prim function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    VOID PrimFree( VOID )
    {
      /* making an array of vertices active */
      glBindVertexArray(VA);
      /* unhooking the buffer */
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDeleteBuffers(1, &VBuf);
      /* making an array of vertices inactive */
      glBindVertexArray(0);
      glDeleteVertexArrays(1, &VA);
      /* making an array of indexes inactive */
      glDeleteBuffers(1, &IBuf);
      ZeroMemory(this, sizeof(prim));
    } /* End of 'PrimFree' function */

    /* Load primitive from '*.OBJ' file function.
     * ARGUMENTS:
     *   - pointer to primitive to load:
     *       ns6PRIM *Pr;
     *   - '*.OBJ' file name:
     *       CHAR *FileName;
     * RETURNS:
     *   (BOOL) TRUE if success, FALSE otherwise.
     */
    BOOL PrimLoad( const CHAR *FileName )
    {
      INT
        noofv = 0,
        noofi = 0,
        size;
      FILE *F;
      CHAR Buf[1000];
      INT *Ind;
      vertex *V;
      
      /* Open file */
      fopen_s(&F, FileName, "r");
      if (F == NULL)
        return FALSE;
      
      /* Count vertex and index quantities */
      while (fgets(Buf, sizeof(Buf) - 1, F) != NULL)
      {
        if (Buf[0] == 'v' && Buf[1] == ' ')
          noofv++;
        else if (Buf[0] == 'f' && Buf[1] == ' ')
          noofi++;
      }
      
      size = sizeof(vertex) * noofv + sizeof(INT) * noofi * 3;
      if ((V = (vertex *)(new vertex[size])) == NULL)
      {
        fclose(F);
        return FALSE;
      }
      Ind = (INT *)(V + noofv);
      
      /* Read vertices and facets data */
      rewind(F);
      noofv = noofi = 0;
      while (fgets(Buf, sizeof(Buf) - 1, F) != NULL)
      {
        if (Buf[0] == 'v' && Buf[1] == ' ')
        {
          FLT x, y, z;
          FLT rnd1 = 1.0 / (rand() % 10), rnd2 = 1.0 / (rand() % 10), rnd3 = 1.0 / (rand() % 10);
      
          sscanf_s(Buf + 2, "%f%f%f", &x, &y, &z);
          V[noofv].P = vec3(x, y, z);
          V[noofv++].C = vec4(rnd1, rnd2, rnd3, 1);
        }
        else if (Buf[0] == 'f' && Buf[1] == ' ')
        {
          INT n1, n2, n3;
      
          /* Read one of possible facet references */
          sscanf_s(Buf + 2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &n1, &n2, &n3) == 3 ||
            sscanf_s(Buf + 2, "%d//%*d %d//%*d %d//%*d", &n1, &n2, &n3) == 3 ||
            sscanf_s(Buf + 2, "%d/%*d %d/%*d %d/%*d", &n1, &n2, &n3) == 3 ||
            sscanf_s(Buf + 2, "%d %d %d", &n1, &n2, &n3);
          n1--;
          n2--;
          n3--;
          Ind[noofi++] = n1;
          Ind[noofi++] = n2;
          Ind[noofi++] = n3;
        }
      }
      
      /* making an auto normalize */
      PrimAutoNorm(V, Ind, noofi);
      
      PrimCreate(nicg::prim_type::PRIM_TRIMESH, V, noofv, Ind, noofi);
      
      fclose(F);
      delete[] V;
      return TRUE;
    } /* End of 'PrimLoad' function */

     /* Auto normal function.
     * ARGUMENTS:
     *   - vertex array:
     *      vertex *V;
     *   - index:
     *      INT *I;
     *   - num of index:
     *      INT NoofI;
     * RETURNS: None.
     */
    static VOID PrimAutoNorm( vertex *V, INT *I, INT NoofI )
    {
      for (INT i = 3; i < NoofI; i += 3)
      {
    
        vec3 N = ((V[I[i + 1]].P - V[I[i]].P) % (V[I[i + 2]].P - V[I[i]].P)).Normalizing();

        V[I[i + 0]].N = ((V[I[i + 0]].N + N)).Normalizing();
        V[I[i + 1]].N = ((V[I[i + 1]].N + N)).Normalizing();
        V[I[i + 2]].N = ((V[I[i + 2]].N + N)).Normalizing();
      }
    } /* End of 'AutoNorm' function */

    VOID EvalBoundBox( vertex *V, INT *ind, INT NoofI )
    {
      for (INT i = 3; i < NoofI; i += 3)
      {
        this->MaxBB.Max(V[ind[i]].P);
        this->MinBB.Min(V[ind[i]].P);
      }
    }

    /* Create primitive from mebius function
     * ARGUMENTS:
     *   - Radius:
     *       FLT R;
     * RETURNS: None.
     */
    VOID PrimCreateMebius( FLT R )
    {
      FLT x, y, z;
      INT cnt = 0;

      vertex VERTEX[1258];
    
      for (FLT i = 0; i <= 2 * PI; i += 0.01)
        for (FLT j = -1; j <= 1; j += 2)
        {
          x = (1 + j * cos(i / 2) / 2) * cos(i) * R;
          y = (1 + j * cos(i / 2) / 2) * sin(i) * R;
          z = j * sin(i / 2) * R / 2;
          VERTEX[cnt].P = vec3(x, y, z);
          VERTEX[cnt].T = vec2(x, y);
          cnt++;
        }
    
      PrimCreate(prim_type::PRIM_TRISTRIP, VERTEX, cnt, nullptr, 0);
    } /* End of 'PrimCreateMebius' fucntion */

    /* Create primitive from mebius function
     * ARGUMENTS:
     *   - Radius:
     *       FLT R;
     * RETURNS: None.
     */
    VOID PrimCreateCylinder( FLT R, FLT Height, vec4 C, INT W, INT H )
    {
      vertex *V = new vertex[W * H];

      INT cnt = 0;
      FLT x, y, z;
      DBL h = 0, phi = 0;

      for (INT i = 0; i < H; i++, h += Height / (H - 1))
        for (INT j = 0; j < W; j++, phi += 2 * PI / (W - 1))
        {
          x = R * sin(phi);
          y = h;
          z = R * cos(phi);
          V[cnt].P = vec3(x, y, z);
          V[cnt].C = C;
          V[cnt].N = vec3(x / R, y, z / R);
          V[cnt].T = vec2(x, y);
          cnt++;
        }

      CreateFromGrid(V, W, H);

      delete[] V;
    } /* End of 'PrimCreateMebius' fucntion */

    VOID GridAutoNorm( vertex *V, INT *Ind, INT NoofI, INT W, INT H )
    {
      INT i, j;

      for (i = 0; i < H - 1; i++)
        for (j = 0; j < W - 1; j++)
        {
          vertex
            *P00 = V + i * W + j,
            *P01 = V + i * W + j + 1,
            *P10 = V + (i + 1) * W + j,
            *P11 = V + (i + 1) * W + j + 1;
          vec3 N;

          N = ((P00->P - P10->P) % (P11->P - P10->P)).Normalizing();
          P00->N = (P00->N + N).Normalizing();
          P10->N = (P10->N + N).Normalizing();
          P11->N = (P11->N + N).Normalizing();

          N = ((P11->P- P01->P) % (P00->P - P01->P)).Normalizing();
          P00->N = (P00->N + N).Normalizing();
          P01->N = (P01->N + N).Normalizing();
          P11->N = (P11->N + N).Normalizing();
        }
    }

    BOOL CreateFromGrid( vertex *V, INT W, INT H )
    {
      INT i, j, k;
      INT *Ind;

      memset(this, 0, sizeof(prim));

      Ind = new INT[(H - 1) * (W * 2 + 1) - 1];

      for (i = 0, k = 0; i < H - 1; i++)
      {
        for (j = 0; j < W; j++)
        {
          Ind[k++] = (i + 1) * W + j;
          Ind[k++] = i * W + j;
        }
        if (i != H - 2)
          Ind[k++] = -1;
      }

      GridAutoNorm(V, Ind, NumOfEl, W, H);
      PrimCreate(prim_type::PRIM_TRISTRIP, V, W * H, Ind, (H - 1) * (W * 2 + 1) - 1);

      delete[] Ind;
      return TRUE;
    }

    VOID PrimCreateSphere( FLT R, const vec3 &C )
    {
      INT i = 0, j = 0;
      stock<vertex> V;


      for (DBL th = 0, i = 0; i < 30; i++, th += PI / 29)
        for (DBL phi = 0, j = 0; j < 30; j++, phi += 2 * PI / 29)
        {
          DBL sit = sin(th), cot = cos(th), sip = sin(phi), cop = cos(phi);
          vertex NV;
          FLT x, y, z;

          x = C[0] + R * sit * sip;
          y = C[1] + R * cot;
          z = C[2] + R * sit * cop;

          NV.P = vec3(x, y, z);
          NV.T = vec2(x, y);


          V.Add(NV);
        }

      CreateFromGrid(V, 30, 30);
    }
  }; /* end of 'prim' class */

  /* Model class */
  class model
  {
  private:
    stock<prim *> Prims; /* Primitives stock */

  public:

    /* Default constructor */
    model( VOID )
    {
    } /* End of 'model' constructor */

    /* Destructor */
    ~model( VOID )
    {
      Prims.Walk([]
      ( prim *pr )
        {
          delete pr;
        });
    } /* End of 'model' destructor */

    /* Load '.g3dm' function.
     * ARGUMENTS:
     *   - filename to load:
     *       CHAR *FileName;
     * RETURNS: (BOOL) TRUE if success, FALSE otherwise
     */
    BOOL Load( const CHAR *FileName );

    /* Walk for all primitives function.
     * ARGUMENTS:
     *   - function to walk:
     *       const type &Walker;
     * RETURNS: None.
     */
    template <typename type>
    VOID WalkAll( const type &Walker )
    {
      this->Prims.Walk(Walker);
    } /* End of 'WalkAll' function */

    /* Walk for all primitives function.
     * ARGUMENTS:
     *   - function to walk:
     *       const type &Walker;
     * RETURNS: None.
     */
    template <typename type>
    VOID WalkAll2( const type &Walker )
    {
      this->Prims.Walk2(Walker);
    } /* End of 'WalkAll' function */

    VOID SetTransMult( matr M )
    {
      for (INT i = 0; i < Prims.Size(); i++)
        Prims[i]->Trans = Prims[i]->Trans * M;
    }

    VOID SetShader( const CHAR *ShaderName );

  }; /* End of 'model' class */

  /* Unstructured data file reader class */
  class rdr
  {
  private:
    // Memory pointer reference
    BYTE *&Ptr;

  public:
    /* Class constructor.
      * ARGUMENTS:
      *   - reference to memory pointer:
      *       BYTE *&PtrRef;
      */
    rdr( BYTE *&PtrRef ) : Ptr(PtrRef)
    {
    } /* End of 'rdr' function */

    /* Read data function.
      * ARGUMENT:
      *   - read data pointer:
      *       type *Data;
      *   - read data count:
      *       INT Count;
      * RETURNS: None.
      */
    template<typename type>
      VOID operator()( type *Data, const INT Count = 1 )
      {
          memcpy(Data, Ptr, sizeof(type) * Count), Ptr += sizeof(type) * Count;
      } /* End of 'operator()' function */
  }; /* End of 'rdr' class */

} /* end of 'nicg' spacename */
#endif /* _render_prim_h_ */

/* END OF 'render_prim.h' FILE */