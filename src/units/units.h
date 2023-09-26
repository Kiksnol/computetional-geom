/* FILE NAME  : units.h
 * PROGRAMMER : ND4
 * LAST UPDATE: 24.07.2021
 * PURPOSE    : WinAPI preferences
 */

#ifndef __units_h_
#define __units_h_

#include "../anim/anim.h"
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <deque>
#include <unordered_set>
#include <set>
//#include "../anim/render/res/sbuffers.h"

#pragma warning(disable: 26451)

namespace nicg
{
  enum MODE
  {
    POINTS,
    SEGMENTS,
    POLYGON_LINES,
    ROAD
  };

  /* Triangle structure */
  struct triangle
  {
    INT I1, I2, I3;

    triangle(VOID) : I1(0), I2(0), I3(0)
    {
    }

    triangle(INT P1, INT P2, INT P3) : I1(P1), I2(P2), I3(P3)
    {
    }
  }; /* End of 'triangle' structure */

    class segment
    {
    public:
      INT St, End;

      segment( VOID )
      {

      }

      segment( INT Start, INT P2 ) : St(Start), End(P2)
      {
      }
    };

    class points_pool
    {
    friend class segment;

    struct polygon
    {
      BOOL IsPolygonStarted = FALSE, IsPolygonEnded = FALSE;
      INT StartIndex = -1, EndIndex = 0;
      INT SegmentStInd = -1, SegmentEndInd = -1;
      std::vector<INT> Ind;
      INT Size = 0;
    };

    private:
      std::vector<vec3> Points;
      polygon RoadPoly;
      std::vector<segment> TrSegments;
      std::vector<vec3> IntResPoints;
      FLT Epsilon = 0.5;
      INT SavePoint = -1;
      INT ActivePoly = 0;
      std::vector<INT> DetachedPoints;
      std::vector<INT> PointsToSetH;
      std::vector<triangle> MamaTriangles;
      material Mt = material("ModelBbera", { 0.25, 0.148, 0.06475 }, { 0.4, 0.2368, 0.1036 }, { 0.774597, 0.458561, 0.200621 }, 76.8, 1, "PAPA");
      material Mt2 = material("ModelBbera", { 0.25, 0.148, 0.06475 }, { 0.4, 0.2368, 0.1036 }, { 0.774597, 0.458561, 0.200621 }, 76.8, 1, "CITY");
      BOOL FirstTime = TRUE;

    public:
      std::vector<segment> Connections;
      std::vector<segment> RoadConnections;
      std::vector<segment> RoadSegs;
      std::vector<vec3> PointsRoad, PointsPolygon;
      std::vector<triangle> RoadTris;
      std::vector<polygon> Polygons;
      INT lastPntInd;
      prim PP, RD, rdSph;
      BOOL IsPrimCreated = FALSE, IsRdCr = FALSE;

      points_pool( VOID )
      {
        Polygons.push_back(polygon());
        Mt.Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/mama.g32", 4);

        rdSph.PrimCreateSphere(1, vec3(0, 0, 0));
        rdSph.Mtl->Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/BRICK.g32", 4);
        rdSph.Mtl->Shd = anim::GetPtr()->shader_manager::Add("ARC170");
      }

      ~points_pool(VOID)
      {
        Points.clear();
      }

      /* Square from triangle function
       * ARGUMENTS:
       *   - triangle:
       *       const triangle &Trg;
       * RETURNS:
       *   (FLT) square;
       */
      FLT TriangleSquare(const triangle &Trg)
      {
        FLT a = (Points[Trg.I2] - Points[Trg.I1]).Length2(),
          b = (Points[Trg.I3] - Points[Trg.I1]).Length2(),
          c = (Points[Trg.I3] - Points[Trg.I1]) & (Points[Trg.I2] - Points[Trg.I1]);

        return 0.5 * sqrt(a) * sqrt(b) * sqrt(1 - (c * c) / (a * b));
      } /* End of 'Square' function */

      BOOL CheckClockWise(polygon Pol)
      {
        INT maxy = 0, n = (INT)Pol.Ind.size();

        if (Pol.Ind.size() < 3)
          return FALSE;
        for (INT i = 1; i < n; i++)
          if (Points[Pol.Ind[i]].Z > Points[Pol.Ind[maxy]].Z)
            maxy = i;
        vec3 a = Points[Pol.Ind[(maxy - 1 + n) % n]] - Points[Pol.Ind[maxy]],
          b = Points[Pol.Ind[(maxy + 1) % n]] - Points[Pol.Ind[maxy]];
        return (a | b) > 0;
      } /* End of 'CheckClockWise' function */

      /* Correct clock-wise polygon orientation function.
       * ARGUMENTS:
       *   - pointer to polygon:
       *       std::vector<vec2> * const Pol;
       * RETURNS: None.
       */
      BOOL ClockWiseCorrect(polygon *Pol)
      {
        if (!CheckClockWise(*Pol))
        {
          std::reverse(Pol->Ind.begin(), Pol->Ind.end());
          return TRUE;
        }
        return FALSE;
      } /* End of 'ClockWiseCorrect' function */

      /* Check is point right function.
       * ARGUMENTS:
       *   - points:
       *       vec3 A, B, C;
       * RETURNS: None.
       */
      BOOL CheckRight(vec3 A, vec3 B, vec3 C)
      {
        if ((C.Z - A.Z) * (B.X - A.X) - (C.X - A.X) * (B.Z - A.Z) <= 0)
          return TRUE;
        return FALSE;
      } /* End of 'CheckRight' function */

      /* Is valid intersection function.
     * ARGUMENTS:
     *   - Polygon indexes:
     *       INT I1, I2, I3;
     *   - Polygon:
     *       polygon Pol;
     * RETURNS: (BOOL) is could intersection.
     */
      BOOL IsVlaidIntersection(INT I1, INT I2, INT I3, polygon Pol)
      {
        for (INT i = 0; i < Pol.Ind.size(); i++)
        {
          if (Pol.Ind[i] == I1 || Pol.Ind[i] == I2 || Pol.Ind[i] == I3)
            continue;
          BOOL
            a = CheckRight(Points[I1], Points[Pol.Ind[i]], Points[I2]),
            b = CheckRight(Points[I2], Points[Pol.Ind[i]], Points[I3]),
            c = CheckRight(Points[I3], Points[Pol.Ind[i]], Points[I1]);

          if (a == b && b == c)
            return FALSE;
        }
        return TRUE;
      } /* End of 'IsValidIntersection' function */

      vec3 PointsGet( INT Ind )
      {
        if (Ind > 0 && Ind < Points.size())
          return Points[Ind];
        else if (Ind < 0)
          return PointsGet(Points.size() + Ind);
        else
          return Points[Ind % Points.size()];
      }

      BOOL FindInDet(INT N)
      {
        for (INT i = 0; i < DetachedPoints.size(); i++)
          if (DetachedPoints[i] == N)
            return TRUE;

        return FALSE;
      }

      BOOL Find( INT N, INT *Ind )
      {
        for (INT i = 0; i < PointsToSetH.size(); i++)
          if (PointsToSetH[i] == N)
          {
            if (Ind != nullptr)
              *Ind = i;

            return TRUE;
          }

        return FALSE;
      }


      /* Triangulate polygon function.
     * ARGUMENTS:
     *   - Polygon:
     *       polygon Pol;
     *   - Starter polygon:
     *       polygon OldPol;
     * RETURNS: (std::vector<triangle>) result triangulation.
     */
      VOID PolyTriangulation2( polygon Pol, const polygon OldPol, std::vector<triangle> &Tri )
      {
        FLT max_square = 0;
        triangle max_triangle;
        INT delete_index = 0;

        if (Pol.Ind.size() == 3)
        {
          Tri.push_back(triangle(Pol.Ind[0], Pol.Ind[1], Pol.Ind[2]));
          return;
        }
        else if (Pol.Ind.size() > 0)
        {
          for (INT i = 0; i < Pol.Ind.size() - 2; i++)
            if (CheckRight(Points[Pol.Ind[i]], Points[Pol.Ind[i + 1]], Points[Pol.Ind[i + 2]]) &&
              IsVlaidIntersection(Pol.Ind[i], Pol.Ind[i + 1], Pol.Ind[i + 2], OldPol))
            {
              FLT s = TriangleSquare(triangle(Pol.Ind[i], Pol.Ind[i + 1], Pol.Ind[i + 2]));

              if (s > max_square)
              {
                max_triangle = triangle(Pol.Ind[i], Pol.Ind[i + 1], Pol.Ind[i + 2]);
                max_square = s;
                delete_index = i + 1;
              }
            }
          Tri.push_back(max_triangle);
          Pol.Ind.erase(Pol.Ind.begin() + delete_index);

          std::vector<triangle> Tmp;
          PolyTriangulation2(Pol, OldPol, Tmp);

          Tri.insert(Tri.end(), Tmp.begin(), Tmp.end());

          return;
        }
      } /* End of 'PolyTriangulation' function */

      /* Find all polygons in intersected polygon function.
     * ARGUMENTS:
     *   - Polygon:
     *       polygon Pol;
     * RETURNS: None.
     */
      VOID FindPolygons(polygon Pol, std::vector<polygon> &Polygons)
      {
        INT cnt = 0;
        for (INT i = 0; i < Pol.Ind.size(); i++)
          for (INT j = i + 1; j < Pol.Ind.size(); j++)
          {
            if (Pol.Ind[i] == Pol.Ind[j])
            {
              polygon P;
              std::vector<polygon> A, B;

              for (INT k = i; k < j; k++)
                P.Ind.push_back(Pol.Ind[k]);
              Pol.Ind.erase(Pol.Ind.begin() + i, Pol.Ind.begin() + j);

              FindPolygons(P, A);
              FindPolygons(Pol, B);

              Polygons.insert(Polygons.end(), A.begin(), A.end());
              Polygons.insert(Polygons.end(), B.begin(), B.end());

              return;
            }
          }

        Polygons = { Pol };
      }  /* End of 'FindPolygons' function */

      vec3 operator[](INT Ind)
      {
        if (Ind < 0 || Ind >(Points.size() - 1))
          return vec3(0);

        return Points[Ind];
      }

      vec3 operator()(INT Ind)
      {
        if (Ind < 0 || Ind >(IntResPoints.size() - 1))
          return vec3(0);

        return IntResPoints[Ind];
      }

      BOOL Find( segment S, INT* Ind )
      {
        for (INT i = 0; i < Connections.size(); i++)
          if (Connections[i].St == S.St && Connections[i].End == S.End)
          {
            if (Ind != nullptr)
              *Ind = i;

            return TRUE;
          }

        return FALSE;
      }

      BOOL Find2(segment S, INT* Ind)
      {
        for (INT i = 0; i < RoadConnections.size(); i++)
          if (RoadConnections[i].St == S.St && RoadConnections[i].End == S.End)
          {
            if (Ind != nullptr)
              *Ind = i;

            return TRUE;
          }

        return FALSE;
      }

      INT Add( vec3 P, MODE M )
      {
        if (M == POLYGON_LINES)
        {
          INT I;
          BOOL IsTurningRight = FALSE;

          if (!Find(P, Epsilon, &I))
          {
            if (Polygons[ActivePoly].IsPolygonEnded)
            {
              Polygons.push_back(polygon());
              ActivePoly++;
            }

            Points.push_back(P);

            INT ind = Points.size() - 1;
            Polygons[ActivePoly].Size++;
            Polygons[ActivePoly].Ind.push_back(ind);

            if (Polygons[ActivePoly].Size > 1)
            {
              Connections.push_back(segment(lastPntInd, Points.size() - 1));
              if (Polygons[ActivePoly].SegmentStInd == -1)
                Polygons[ActivePoly].SegmentStInd = Connections.size() - 1;

              lastPntInd = ind;
            }

            if (Polygons[ActivePoly].IsPolygonStarted == FALSE)
            {
              Polygons[ActivePoly].IsPolygonStarted = TRUE;
              Polygons[ActivePoly].StartIndex = ind;
              lastPntInd = ind;
            }

            return ind;
          }
          else
          {
            segment S = segment(lastPntInd, I);

            if (S.St == S.End)
              return -1;

            if (Points.size() > 1 && !Find(S, nullptr))
            {
              Connections.push_back(segment(lastPntInd, I));

              if (I == Polygons[ActivePoly].StartIndex)
              {
                Polygons[ActivePoly].EndIndex = lastPntInd;
                Polygons[ActivePoly].SegmentEndInd = Connections.size() - 1;
                Polygons[ActivePoly].IsPolygonEnded = TRUE;
              }
            }

            return I;
          }
        }
        else if (M == POINTS && !Find(P, Epsilon, nullptr))
          Points.push_back(P);
        else if (M == SEGMENTS)
        {
          INT Ind;

          if (!Find(P, Epsilon, &Ind))
          {
            Points.push_back(P);

            if (Points.size() % 2 == 0 && SavePoint == -1)
              Connections.push_back(segment(Points.size() - 2, Points.size() - 1));
            else if (Points.size() % 2 == 0 && SavePoint != -1)
            {
              Connections.push_back(segment(SavePoint, Points.size() - 1));
              SavePoint = -1;
            }

            return Points.size() - 1;
          }
          else
            if (SavePoint == -1)
              SavePoint = Ind;
            else
            {
              Connections.push_back(segment(SavePoint, Ind));
              SavePoint = -1;
            }
        }
        else if (M == ROAD)
        {
          INT I;
          BOOL IsTurningRight = FALSE;

          if (!Find2(P, Epsilon, &I))
          {
            PointsPolygon.push_back(P);

            INT ind = PointsPolygon.size() - 1;
            RoadPoly.Size++;
            RoadPoly.Ind.push_back(ind);

            vec3 P, Perp1, p0, p1, p2, p3;

            if (PointsPolygon.size() > 1)
            {
              P = PointsPolygon[lastPntInd] - PointsPolygon[ind], Perp1 = vec3(-P.Z, 0, P.X).Normalizing() * 3;
              p0 = PointsPolygon[lastPntInd] + Perp1, p1 = PointsPolygon[lastPntInd] - Perp1, p2 = PointsPolygon[ind] + Perp1, p3 = PointsPolygon[ind] - Perp1;
            }


            if (RoadPoly.Size > 1)
            {
              RoadConnections.push_back(segment(lastPntInd, PointsPolygon.size() - 1));
              if (RoadPoly.SegmentStInd == -1)
                RoadPoly.SegmentStInd = RoadConnections.size() - 1;

              lastPntInd = ind;
            }

            if (RoadPoly.IsPolygonStarted == FALSE)
            {
              RoadPoly.IsPolygonStarted = TRUE;
              RoadPoly.StartIndex = ind;
              lastPntInd = ind;
            }

            if (PointsPolygon.size() > 1)
            {
              PointsRoad.push_back(p0);
              PointsRoad.push_back(p1);
              RoadSegs.push_back(segment(PointsRoad.size() - 2, PointsRoad.size() - 1));
              PointsRoad.push_back(p2);
              PointsRoad.push_back(p3);
              RoadSegs.push_back(segment(PointsRoad.size() - 2, PointsRoad.size() - 1));
              RoadSegs.push_back(segment(PointsRoad.size() - 4, PointsRoad.size() - 2));
              RoadSegs.push_back(segment(PointsRoad.size() - 3, PointsRoad.size() - 1));
              PointsRoad.push_back(PointsPolygon[PointsPolygon.size() - 1]);
              RoadSegs.push_back(segment(PointsRoad.size() - 5, PointsRoad.size() - 1));
              RoadSegs.push_back(segment(PointsRoad.size() - 4, PointsRoad.size() - 1));
              RoadTris.push_back(triangle(PointsRoad.size() - 5, PointsRoad.size() - 1, PointsRoad.size() - 3));
              RoadTris.push_back(triangle(PointsRoad.size() - 4, PointsRoad.size() - 1, PointsRoad.size() - 2));
              RoadTris.push_back(triangle(PointsRoad.size() - 4, PointsRoad.size() - 1, PointsRoad.size() - 5));

              if (PointsRoad.size() > 5)
              {
                vec3 Lpnt = PointsPolygon[lastPntInd], A = PointsPolygon[RoadConnections[RoadConnections.size() - 2].St], B = PointsPolygon[RoadConnections[RoadConnections.size() - 2].End];

                IsTurningRight = CheckRight(Lpnt, B, A);

                if (FirstTime)
                {
                  if (IsTurningRight)
                  {
                    vec3 CP = (PointsRoad[PointsRoad.size() - 5] + PointsRoad[PointsRoad.size() - 8]) / 2, D = (CP - PointsRoad[PointsRoad.size() - 6]).Normalizing();
                    CP = CP + D * 0.5;
                    PointsRoad.push_back(CP);
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 6));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 9));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 8));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 8, PointsRoad.size() - 6));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 8, PointsRoad.size() - 9));
                  }
                  else
                  {
                    vec3 DP = (PointsRoad[PointsRoad.size() - 4] + PointsRoad[PointsRoad.size() - 7]) / 2, D = (DP - PointsRoad[PointsRoad.size() - 6]).Normalizing();
                    DP = DP + D * 0.5;
                    PointsRoad.push_back(DP);
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 5));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 8));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 7));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 7, PointsRoad.size() - 5));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 7, PointsRoad.size() - 8));
                  }

                  FirstTime = FALSE;
                }
                else
                  if (IsTurningRight)
                  {
                    vec3 CP = (PointsRoad[PointsRoad.size() - 5] + PointsRoad[PointsRoad.size() - 9]) / 2, D = (CP - PointsRoad[PointsRoad.size() - 6]).Normalizing();
                    CP = CP + D * 0.5;
                    PointsRoad.push_back(CP);
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 6));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 10));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 8));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 8, PointsRoad.size() - 6));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 8, PointsRoad.size() - 10));
                  }
                  else
                  {
                    vec3 DP = (PointsRoad[PointsRoad.size() - 4] + PointsRoad[PointsRoad.size() - 8]) / 2, D = (DP - PointsRoad[PointsRoad.size() - 6]).Normalizing();
                    DP = DP + D * 0.5;
                    PointsRoad.push_back(DP);
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 5));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 9));
                    RoadSegs.push_back(segment(PointsRoad.size() - 1, PointsRoad.size() - 8));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 8, PointsRoad.size() - 5));
                    RoadTris.push_back(triangle(PointsRoad.size() - 1, PointsRoad.size() - 8, PointsRoad.size() - 9));
                  }
              }
            }

            return ind;
          }
          else
          {
            segment S = segment(lastPntInd, I);

            if (S.St == S.End)
              return -1;

            if (PointsPolygon.size() > 1 && !Find2(S, nullptr))
            {
              RoadConnections.push_back(segment(lastPntInd, I));

              if (I == RoadPoly.StartIndex)
              {
                RoadPoly.EndIndex = lastPntInd;
                RoadPoly.SegmentEndInd = RoadConnections.size() - 1;
                RoadPoly.IsPolygonEnded = TRUE;
              }
            }

            return I;
          }
        }
      return -1;
      }

      BOOL FindInSeg( INT Ind )
      {
        for (INT i = 0; i < Connections.size(); i++)
          if (Connections[i].St == Ind || Connections[i].End == Ind)
            return TRUE;

        return FALSE;
      }

      INT PopLastPoint( MODE M )
      {
        if (M == POLYGON_LINES)
        {
          if (Connections.size() > 0 && Connections[Connections.size() - 1].End != Points.size() - 1)
          {
            Connections.pop_back();
            return -1;
          }
        
          if (Points.size() > 0)
          {
            Points.pop_back();
            if (Points.size() > 0)
              Connections.pop_back();
          }
        
          return Points.size();
        }
        else if (M == POINTS)
        {
          if (Points.size() > 0 && !FindInSeg(Points.size() - 1))
            Points.pop_back();
        }
        else if (M == SEGMENTS)
        {
          INT I1, I2;
          if (Connections.size() > 0)
          {
            I1 = Connections[Connections.size() - 1].St, I2 = Connections[Connections.size() - 1].End;
            Connections.pop_back();
          }
        }

        return -1;
      }

      /* Sweep line function.
     * ARGUMENTS: None.
     * RETURNS: None.
     */
      VOID SweepLine(VOID)
      {
        IntResPoints.clear();
        std::vector<FLT> StartCoord;

        std::sort(Connections.begin(), Connections.end(), [&](const segment& a, const segment& b)
          {
            return this->Points[a.St].X > this->Points[b.St].X;
          });

        for (INT i = 0; i < Connections.size() - 1; i++)
        {
          INT j = i + 1;

          while (Points[Connections[i].End].X >= Points[Connections[j].St].X)
          {
            vec3 P;

            if (SegmentIntersectSegment(Points[Connections[i].St], Points[Connections[i].End],
              Points[Connections[j].St], Points[Connections[j].End], &P))
            {
              IntResPoints.push_back({ P });
            }

            j++;
            if (j == Connections.size())
              break;
          }
        }
      } /* End of 'SweepLine' function */

      VOID MovePoint( vec3 P )
      {
        // Checking if points is real
        INT PointInd;

        if (Find(P, Epsilon * 2, &PointInd))
        {
          Points[PointInd] = P;
        }
      }

      INT GetSize( VOID )
      {
        return Points.size();
      }

      INT GetIntSize(VOID)
      {
        return IntResPoints.size();
      }

      vec3 Get( INT Ind )
      {
        return Points[Ind];
      }

      BOOL Find( vec3 P, FLT Rad, INT *Index )
      {
        for (INT i = 0; i < Points.size(); i++)
          if (Points[i].Distance(P) < Rad)
          {
            if (Index != nullptr)
              *Index = i;

            return TRUE;
          }

        return FALSE;
      }

      BOOL Find2(vec3 P, FLT Rad, INT* Index)
      {
        for (INT i = 0; i < PointsPolygon.size(); i++)
          if (PointsPolygon[i].Distance(P) < Rad)
          {
            if (Index != nullptr)
              *Index = i;

            return TRUE;
          }

        return FALSE;
      }

      vec3 Bisector( vec3 &A, vec3 &B, vec3 &C )
      {
        vec3 a = (A - B).Normalizing();
        vec3 c = (C - B).Normalizing();
        vec3 D = (c - a) / 2;

        return a + D;
      }

      BOOL SegmentIntersectSegment( vec3 A, vec3 B, vec3 C, vec3 D, vec3 *Res )
      {
        FLT m1 = A[0] - B[0], m2 = A[2] - B[2],
          e1 = D[0] - C[0], e2 = D[2] - C[2],
          n1 = D[0] - B[0], n2 = D[2] - B[2];
        FLT det;

        if ((det = e1 * m2 - e2 * m1) != 0)
        {
          FLT t1, t2;

          t2 = (m2 * n1 - n2 * m1) / det;
          t1 = (n2 * e1 - e2 * n1) / det;
          if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
          {
            if (Res != nullptr)
              *Res = A * t1 + B * (1 - t1);
            return TRUE;
          }
          return FALSE;
        }
        // segments are collinear
        return FALSE;
      }

      VOID CheckForInterSections( VOID )
      {
        IntResPoints.clear();

        for (INT i = 0; i < Connections.size(); i++)
          for (INT j = 0; j < Connections.size(); j++)
          {
            if (i == j)
              continue;

            vec3 A = Points[Connections[i].St], B = Points[Connections[i].End];
            vec3 C = Points[Connections[j].St], D = Points[Connections[j].End], R = vec3(0);

            if (SegmentIntersectSegment(A, B, C, D, &R) && !Find(R, Epsilon, nullptr))
              IntResPoints.push_back(R);
          }

      }

      BOOL CheckPolygonForSelfIntersection( polygon &Poly )
      {
        for (INT i = Poly.SegmentStInd; i < Poly.SegmentEndInd; i++)
          for (INT k = Poly.SegmentStInd; k < Poly.SegmentEndInd; k++)
          {
            if (k == i)
              continue;

            vec3 P;
            if (SegmentIntersectSegment(Points[Connections[i].St], Points[Connections[i].End], Points[Connections[k].St], Points[Connections[k].End], &P) && !Find(P, Epsilon, nullptr))
              return TRUE;
          }

        return FALSE;
      }

      /* Is valid intersection function.
       * ARGUMENTS:
       *   - Polygon indexes:
       *       INT I1, I2, I3;
       *   - Polygon:
       *       polygon Pol;
       * RETURNS: (BOOL) is could intersection.
       */
      BOOL CheckPolygonSelfIntersection(polygon& Pol)
      {
        INT Size = Pol.Ind.size(), cnt = 0;
        polygon OldPol = Pol;

        for (INT i = 0; i < Size; i++)
          for (INT j = i + 2; j < Size; j++)
          {
            vec3 P;
            INT k = (i + 1) % Size, t = (j + 1) % Size;

            if (i == t || k == j)
              continue;

            if (SegmentIntersectSegment(Points[OldPol.Ind[i]], Points[OldPol.Ind[k]], Points[OldPol.Ind[j]], Points[OldPol.Ind[t]], &P))
            {
              cnt++;
              Points.push_back(P);

              INT EndInd = j,
                StInd = k, d = EndInd - StInd;

              for (INT l = 0; l < Pol.Ind.size(); l++)
              {
                if (OldPol.Ind[StInd] == Pol.Ind[l])
                {
                  StInd = l;
                  EndInd = StInd + d;

                  break;
                }
                if (OldPol.Ind[EndInd] == Pol.Ind[l])
                {
                  StInd = l;
                  EndInd = StInd + d;

                  break;
                }
              }

              reverse(Pol.Ind.begin() + StInd, Pol.Ind.begin() + EndInd + 1);
              Pol.Ind.insert(Pol.Ind.begin() + StInd, Points.size() - 1);
              Pol.Ind.insert(Pol.Ind.begin() + EndInd + 2, Points.size() - 1);
            }
          }

        ClockWiseCorrect(&Pol);

        return cnt;
      } /* End of 'CheckPolygonIntersection' func0tion */

      /* Add Triangle into triangle vector.
       * ARGUMENTS:
       *   - vertexes indexes:
       *     INT i1, i2, i3;
       * RETURNS: None.
       */
      VOID AddTriangle( INT i1, INT i2, INT i3 )
      {
        if (!Find(segment(i1, i2), nullptr) && !Find(segment(i2, i1), nullptr))
          Connections.push_back(segment(i1, i2));
        if (!Find(segment(i3, i2), nullptr) && !Find(segment(i2, i3), nullptr))
          Connections.push_back(segment(i3, i2));
        if (!Find(segment(i3, i1), nullptr) && !Find(segment(i1, i3), nullptr))
          Connections.push_back(segment(i3, i1));
      }

      /* Triangulate latest polygon function..
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID Triangulate( VOID )
      {
        for (INT i = 0; i < Polygons.size(); i++)
          if (Polygons[i].IsPolygonEnded)
          {
            ClockWiseCorrect(&Polygons[i]);

            CheckPolygonSelfIntersection(Polygons[Polygons.size() - 1]);
            
            std::vector<polygon> P;
            FindPolygons(Polygons[Polygons.size() - 1], P);

            for (auto p : P)
            {
              std::vector<triangle> Tri;

              PolyTriangulation2(p, p, Tri);

              for (INT i = 0; i < Tri.size(); i++)
                AddTriangle(Tri[i].I1, Tri[i].I2, Tri[i].I3);
            }

            /*ChangeCW(&Polygons[i]);

            std::vector<triangulation::ind_triangle> Additive = PolyTriangulation(Polygons[i], Polygons[i], TRUE);
            TriangulationTris.insert(TriangulationTris.end(), Additive.begin(), Additive.end());*/
          }
      }

      /* Count and create bisectors in polygon.
       * ARGUMENTS: 
       *   - polygon to count bisectors:
       *     polygon Poly;
       * RETURNS: None.
       */
      VOID CountBisectorsForEveryAngle( polygon Poly )
      {
        if (Poly.IsPolygonEnded)
        {
          ClockWiseCorrect(&Poly);

          vec3 P, S1, S2;

          for (INT i = 0; i < Poly.Ind.size(); i++)
          {
            P = PointsGet(i);

            S1 = (PointsGet(i - 1));
            S2 = (PointsGet(i + 1));

            vec3 A = Bisector(S1, P, S2), Biss = A * 30 + P;
            


            if (!IsPointInside(Biss, Poly))
              Biss = -A * 30 + P;
            else
              Biss = A * 30 + P;

            Points.push_back(Biss);
            Connections.push_back(segment(i, Points.size() - 1));
          }
        }
      }

      VOID FullClear( VOID )
      {
        Connections.clear();
        Points.clear();
        PointsPolygon.clear();
        PointsRoad.clear();
        RoadPoly.Ind.clear();
        RoadPoly = polygon();
        Polygons.clear();
        Polygons.push_back(polygon());
        RoadSegs.clear();
        RoadTris.clear();
        RoadConnections.clear();
        MamaTriangles.clear();
        PointsToSetH.clear();
        DetachedPoints.clear();
        TrSegments.clear();
        IntResPoints.clear();

      }

      /* Add/delete point to/from height setting vector.
       * ARGUMENTS:
       *   - point to add to height setting vector:
       *     vec3 P;
       * RETURNS: None.
       */
      VOID AddPointToHeightSet( vec3 P )
      {
        INT Ind = -1;

        if (Find(P, Epsilon, &Ind))
        {
          INT n = 0;

          if (!Find(Ind, &n))
            PointsToSetH.push_back(Ind);
          else
            PointsToSetH.erase(PointsToSetH.begin() + n);
        }
      }

      /* Get point with lowest Z value.
       * ARGUMENTS: 
       *   - pointer to index of point:
       *     INT *Ind;
       * RETURNS: 
       *   (vec3) point with lowest Z value.
       */
      vec3 FindLowestZPoint( INT *Ind )
      {
        if (Points.size() > 0)
        {
          vec3 min = Points[0];
          INT min_ind = 0;

          for (INT i = 1; i < Points.size(); i++)
            if (min.Z > Points[i].Z)
              min = Points[i], min_ind = i;

          if (Ind != nullptr)
            *Ind = min_ind;

          return min;
        }
        else
          return vec3(0);
      }

      /* Set point by index to start of points pool.
       * ARGUMENTS: 
       *   - index of point:
       *     INT I;
       * RETURNS: None.
       */
      VOID SetPointToStart( INT I )
      {
        vec3 P = Points[I];

        Points.erase(Points.begin() + I);
        Points.insert(Points.begin(), P);
      }

      /* Swap vec3 values function.
       * ARGUMENTS: 
       *   - pointers to vec3 to swap:
       *     vec3 *A, *B;
       * RETURNS: None.
       */
      VOID Swap( vec3 *A, vec3 *B )
      {
        vec3* tmp = A;

        A = B;
        B = tmp;
      }

      /* Sort points pool by angle to X axis.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID SortPointsByAngle( VOID )
      {
        for (INT i = 1; i < Points.size(); i++)
          for (INT j = 1; j < Points.size(); j++)
          {
            if (i == j)
              continue;

            FLT A = atan2(Points[i].Z - Points[0].Z, Points[i].X - Points[0].X) * 180 / PI, B = atan2(Points[j].Z - Points[0].Z, Points[j].X - Points[0].X) * 180 / PI;

            if ((A < B))
              Swap(&Points[i], &Points[j]);
          }
      }

      /* Check if turn is clock wise or not.
       * ARGUMENTS:
       *   - segment points:
       *     vec3 A, B;
       *   - point to turn:
       *     vec3 C;
       * RETURNS: 
       *   (INT) -1 if CW, 1 if CCW.
       */
      INT CheckClockWise(vec3 A, vec3 B, vec3 C)
      {
        FLT Area = (B.X - A.X) * (C.Z - A.Z) - (B.Z - A.Z) * (C.X - A.X);

        if (Area < 0)
          return -1; // CW
        if (Area > 0.000001)
          return 1; // Counter CW

        // Collinear
        return 0;
      }

      /* Create convex hull with MinMax algorithm.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID CreateConvexHull2( VOID )
      {
        std::vector<vec3> A = ConvexHull(Points);
        std::vector<INT> Ind;

        for (INT i = 0; i < A.size(); i++)
        {
          INT I = 0;

          Find(A[i], Epsilon, &I);

          Ind.push_back(I);
        }

        for (INT i = 0; i < Ind.size(); i++)
          if (i == Ind.size() - 1)
            Connections.push_back(segment(Ind[i], Ind[0]));
          else
            Connections.push_back(segment(Ind[i], Ind[i + 1]));
      }

      /* Convex hull creation with Graham algorithm.
       * ARGUMENTS:
       *   - points to create convex hull:
       *     std::vector<vec3> Pnt;
       * RETURNS: 
       *   (std::vector<vec3>) Points to connect.
       */
      std::vector<vec3> ConvexHull(std::vector<vec3> Pnt)
      {
        std::vector<vec3> P;

        std::sort(Pnt.begin(), Pnt.end(), [&](const vec3 &a, const vec3 &b)
          {
            return a.Z < b.Z;
          });

        P.push_back(Pnt[0]);
        INT last = 0;

        for (INT i = 1; i < Pnt.size(); i++)
        {
          INT cnt = 0;

          for (INT j = 0; j < Pnt.size(); j++)
          {
            if (i == j || j == last)
              continue;
            if (!CheckRight(Pnt[last], Pnt[j], Pnt[i]))
            {
              cnt++;
              break;
            }
          }

          if (cnt == 0)
          {
            P.push_back(Pnt[i]);
            last = i;
          }
        }

        std::reverse(Pnt.begin(), Pnt.end());
        last = 0;

        for (INT i = 1; i < Pnt.size(); i++)
        {
          INT cnt = 0;

          for (INT j = 0; j < Pnt.size(); j++)
          {
            if (i == j || j == last)
              continue;
            if (!CheckRight(Pnt[last], Pnt[j], Pnt[i]))
            {
              cnt++;
              break;
            }
          }

          if (cnt == 0)
          {
            P.push_back(Pnt[i]);
            last = i;
          }
        }

        return P;
      } /* End of 'ConvexHull' function */

      /* Create convex hull with Graham algorithm.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID CreateConvexHull( VOID )
      {
        INT Ind = -1;
        vec3 LP = FindLowestZPoint(&Ind);
        std::deque<INT> Indexes;

        SetPointToStart(Ind);

        //SortPointsByAngle();
        Points.erase(Points.begin());
        std::sort(Points.begin(), Points.end(), [&](const vec3 &a, const vec3 &b)
          {
            return atan2(a.X - LP.X, a.Z - LP.Z) < atan2(b.X - LP.X, b.Z - LP.Z);
          });
        Points.insert(Points.begin(), LP);

        Indexes.push_back(0);
        Indexes.push_back(1);

        INT i = 2;
        while (i < Points.size())
        {
          INT a = i - 2, b = i - 1;

          while (FindInDet(b))
            b--;

          while (FindInDet(a) || a == b)
            a--;

          if (CheckClockWise(Points[i], Points[b], Points[a]) == 1)
          {
            Indexes.push_back(i);
            i++;
          }
          else
          {
            if (Indexes.size() > 2)
            {
              Indexes.pop_back();
              DetachedPoints.push_back(b);
            }
            else
            {
              DetachedPoints.push_back(i);
              i++;
            }
          }
        }

        for (INT k = 0; k < Indexes.size(); k++)
          if (k == Indexes.size() - 1)
            Connections.push_back(segment(Indexes[k], Indexes[0]));
          else
            Connections.push_back(segment(Indexes[k], Indexes[k + 1]));
      }

      /* Ray intersection function.
       * ARGUMENTS:
       *   - first ray org and dir:
       *     vec3 A, Dir1;
       *   - second ray org and dir:
       *     vec3 B, Dir2;
       *   - pointer to intersection point:
       *     FLT *t;
       * RETURNS: 
       *   (BOOL) Is intersect.
       */
      BOOL RayIntrsection( vec3 A, vec3 Dir1, vec3 B, vec3 Dir2, FLT *t = nullptr )
      {
        FLT det = Dir1.X * Dir2.Z - Dir1.Z * Dir2.X;
        FLT dy = A.Z - B.Z, dx = A.X - B.X;

        if (det != 0)
        {
          FLT t1, t2;

          t1 = (Dir2.X * dy - Dir2.Z * dx) / det;
          t2 = (Dir1.X * dy - Dir1.Z * dx) / det;

          if (t != nullptr)
            *t = t1;
          return TRUE;
        }
        // segments are collinear
        return FALSE;
      } /* End of 'RayIntrsection' function */

      /* Point set triangulation for Indexes vector.
       * ARGUMENTS:
       *   - Indexes vector:
       *     std::vector<INT> Pnt;
       * RETURNS: None.
       */
      VOID CreatePointSetTriangulation(std::vector<INT> Pnt)
      {
        struct edge
        {
          INT P1, P2;

          edge(INT a, INT b) : P1(a), P2(b)
          {
          }
        };

        struct Less
        {
          BOOL operator()(edge a, edge b) const // <
          {
            return a.P2 == b.P2 ? a.P1 > b.P1 : a.P2 > b.P2;
          }
        };

        std::vector<edge> Life;
        std::set<edge, Less> Death;
        std::vector<triangle> Tr;

        for (INT i = 1; i < Pnt.size(); i++)
        {
          if (Points[Pnt[i]].Z < Points[Pnt[0]].Z)
            std::swap(Pnt[i], Pnt[0]);
          else if (Points[Pnt[i]].Z == Points[Pnt[0]].Z)
            if (Points[Pnt[i]].X < Points[Pnt[0]].X)
              std::swap(Pnt[i], Pnt[0]);
        }

        std::sort(Pnt.begin(), Pnt.end(), [&](INT a, INT b)
          {
            return ((Points[a] - Points[Pnt[0]]) % (Points[b] - Points[Pnt[0]])).Y > 0;
          });

        Life.emplace_back(edge(Pnt[0], Pnt[1]));

        while (!Life.empty())
        {
          edge cur = Life.back();

          Life.pop_back();

          if (Death.find({ cur.P2, cur.P1 }) != Death.end() ||
            Death.find({ cur.P1, cur.P2 }) != Death.end())
            continue;

          std::vector<INT> T;
          FLT min_t = HUGE_VAL;
          vec3
            a = Points[cur.P1],
            b = Points[cur.P2],
            Org = (a + b) / 2,
            c = (b - a).Normalizing(),
            Dir = vec3(c.Z, 0, -c.X);

          for (INT i = 0; i < Pnt.size(); i++)
          {
            if (Pnt[i] == cur.P2 || Pnt[i] == cur.P1)
              continue;

            vec3
              ai = Points[Pnt[i]],
              Orgi = (ai + b) / 2,
              ci = (ai - b).Normalizing(),
              Diri = vec3(ci.Z, 0, -ci.X);

            if (((ai - a) % (b - a)).Y < 0)
            {
              FLT t;

              if (RayIntrsection(Org, Dir, Orgi, Diri, &t))
              {
                if (t < min_t)
                {
                  T.clear();
                  min_t = t;
                  T.push_back(Pnt[i]);
                }
                else if (t == min_t)
                  T.push_back(Pnt[i]);
              }
            }
          }

          if (T.size() == 1)
          {
            Tr.emplace_back(T[0], cur.P1, cur.P2);
            Life.emplace_back(T[0], cur.P2);
            Life.emplace_back(cur.P1, T[0]);
            Death.emplace(cur.P1, cur.P2);
          }
          else if (T.size() > 1)
          {
            Tr.emplace_back(T[0], cur.P1, cur.P2);
            for (INT i = 0; i < T.size() - 1; i++)
              Life.emplace_back(T[i], T[i + 1]);
            Life.emplace_back(cur.P1, T[0]);
            Death.emplace(cur.P1, cur.P2);
          }
        }

        for (INT i = 0; i < Tr.size(); i++)
        {
          if (!Find(segment(Tr[i].I1, Tr[i].I2), nullptr) && !Find(segment(Tr[i].I2, Tr[i].I1), nullptr))
            Connections.push_back(segment(Tr[i].I1, Tr[i].I2));

          if (!Find(segment(Tr[i].I1, Tr[i].I3), nullptr) && !Find(segment(Tr[i].I3, Tr[i].I1), nullptr))
            Connections.push_back(segment(Tr[i].I1, Tr[i].I3));

          if (!Find(segment(Tr[i].I3, Tr[i].I2), nullptr) && !Find(segment(Tr[i].I2, Tr[i].I3), nullptr))
            Connections.push_back(segment(Tr[i].I3, Tr[i].I2));
        }

        MamaTriangles = Tr;
      }

      /* Point set triangulation for Points vector.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID CreatePointSetConvexHull2(VOID)
      {
        std::vector<INT> pts;

        for (INT i = 0; i < Points.size(); i++)
          pts.push_back(i);

        CreatePointSetTriangulation(pts);
      }

      /* Save to file function.
       * ARGUMENTS:
       *   - Name:
       *       std::string FName;
       * RETURNS: None.
       */
      VOID SaveToFile(std::string FName)
      {
        if (Polygons.size() == 0)
          return;

        std::ofstream File(FName);

        for (auto& p : Polygons)
        {
          File << "BREAKLINE " << p.Ind.size() << "\n";

          for (INT i = 0; i < p.Ind.size(); i++)
          {
            FLT Ytowrite = Points[p.Ind[i]].Y < 0.01 ? 0 : Points[p.Ind[i]].Y;
            File << Points[p.Ind[i]].X << " " <<
              Ytowrite << " " <<
              Points[p.Ind[i]].Z << "\n";
          }
        }

        File.close();
      } /* End 'SaveToFile' function */

      /* Load from file function.
       * ARGUMENTS:
       *   - Filename:
       *     std::string FName;
       * RETURNS: None.
       */
      VOID LoadFromFile( std::string FName )
      {
        Connections.clear();
        Points.clear();
        Polygons.clear();
        Polygons.push_back(polygon());
        IntResPoints.clear();
        TrSegments.clear();

        std::ifstream File(FName);
        std::string line;

        if (File.good())
        {
          while (getline(File, line))
          {
            std::string type;
            int cnt;

            std::stringstream A(line);

            A >> type >> cnt;

            if (type == "BREAKLINE")
            {
              INT StInd = 0;
              for (INT i = 0; i < cnt; i++)
              {
                getline(File, line);
                std::stringstream A(line);
                FLT x, y, z;

                A >> x >> y >> z;
                Points.push_back(vec3(x, y, z));
                if (i > 0)
                  Connections.push_back(segment(Points.size() - 1, Points.size() - 2));
                else
                  StInd = Points.size() - 1;
              }
              Connections.push_back(segment(StInd, Points.size() - 1));
              Polygons[0].StartIndex = StInd;
              Polygons[0].EndIndex = Points.size() - 1;
              Polygons[0].IsPolygonEnded = TRUE;
              Polygons[0].IsPolygonStarted = TRUE;
              Polygons[0].SegmentStInd = 0;
              Polygons[0].SegmentEndInd = Connections.size() - 1;
              
              for (INT i = StInd; i < Points.size() - 1; i++)
                Polygons[0].Ind.push_back(i);

              Triangulate();
            }
            else if (type == "POINTS")
              for (INT i = 0; i < cnt; i++)
              {
                getline(File, line);
                std::stringstream A(line);
                FLT x, y, z;

                A >> x >> y >> z;
                Points.push_back(vec3(x, y, z));
              }
          }
        }

        File.close();
      }

      /* Save file function.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID SaveFile(VOID)
      {
        OPENFILENAME ofn;
        char fileName[MAX_PATH] = "";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFilter = "Text Files (*.txt)";
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "";
        ofn.lpstrInitialDir = "BIN\\points";

        std::string fileNameStr;
        if (GetSaveFileName(&ofn))
          fileNameStr = fileName;

        SaveToFile(fileNameStr);
      } /* End 'SaveFile' function */

      /* Load file function.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID LoadFile(VOID)
      {
        OPENFILENAME ofn;
        char fileName[MAX_PATH] = "";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFilter = "Text Files (*.txt)";
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "";
        ofn.lpstrInitialDir = "BIN\\points";

        std::string fileNameStr;
        if (GetOpenFileName(&ofn))
          fileNameStr = fileName;

        LoadFromFile(fileNameStr);
      } /* End 'SaveFile' function */

      /* Open file function.
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID OpenFile(VOID)
      {
        OPENFILENAME ofn;
        char fileName[MAX_PATH] = "";

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFilter = "Text Files (*.txt)";
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "";
        ofn.lpstrInitialDir = "bin\\points";

        std::string fileNameStr;
        if (GetOpenFileName(&ofn))
          fileNameStr = fileName;
      } /* End 'OpenFile' function */

      /* Check if point inside polygon.
       * ARGUMENTS:
       *   - point to check:
       *       const vec3 &Pnt;
       *   - polygon to check:
       *       polygon Poly;
       * RETURNS:
       *   (BOOL) whether point in polygon or not.
       */
      BOOL IsPointInside( const vec3 &Pnt, polygon Poly )
      {
        INT cnt = 0;
        vec3 PntInfLine = Pnt + vec3(10000000, 0, 0);
        
        for (INT i = Poly.SegmentStInd; i < Poly.SegmentEndInd; i++)
        {
          segment S = Connections[i];
          cnt += SegmentIntersectSegment(Pnt, PntInfLine, Points[S.St], Points[S.End], nullptr);
        }

        if (cnt % 2 != 0)
          return TRUE;

        return FALSE;
      }

      /* Add Y for chosen points
       * ARGUMENTS: 
       *   - Y value to add:
       *     FLT N;
       * RETURNS: None.
       */
      VOID AddToCurrentHeightPoints( FLT N )
      {
        for (INT i = 0; i < PointsToSetH.size(); i++)
          Points[PointsToSetH[i]].Y += N;
      }

      /* Clear Height for chosen points
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID ClearHeightSet( VOID )
      {
        PointsToSetH.clear();
      }

      /* Create primitive from points pool
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID CreatePrimitiveFromPP( VOID )
      {
        INT Size = MamaTriangles.size() * 3;

        vertex *V;

        V = new vertex[Size];

        INT k = 0;
        for (INT i = 0; i < MamaTriangles.size(); i++)
        {
          triangle T = MamaTriangles[i];

          V[k].P = Points[T.I1];
          V[k].T = vec2(0, 0);
          V[k].N = vec3(0, 1, 0);
          V[k++].C = vec4(0);

          V[k].P = Points[T.I2];
          V[k].T = vec2(0, 0);
          V[k].N = vec3(0, 1, 0);
          V[k++].C = vec4(0);

          V[k].P = Points[T.I3];
          V[k].T = vec2(0, 0);
          V[k].N = vec3(0, 1, 0);
          V[k++].C = vec4(0);
        }

        INT *Ind;
        Ind = new INT[Size];

        for (INT i = 0; i < Size; i++)
          Ind[i] = i;

        PP.PrimAutoNorm(V, Ind, Size);
        PP.PrimCreate(prim_type::PRIM_TRIMESH, V, Size, Ind, Size);

        PP.Mtl = &Mt;
        IsPrimCreated = TRUE;

        delete[] Ind;
        delete[] V;
      }

      VOID Loft( VOID )
      {
        INT n = Points.size(), j = Connections.size();

        for (INT i = 0; i < n; i++)
        {
          vec3 P = Points[i];

          P.Y = 10;

          Points.push_back(P);

          Connections.push_back(segment(i, Points.size() - 1));
        }

        for (INT i = 0; i < j; i++)
          Connections.push_back(segment(Connections[i].St + n, Connections[i].End + n));
      }

      /* Create primitive from road points
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      VOID CreateRoadPrim( VOID )
      {
        INT Size = RoadTris.size() * 3;

        vertex* V;

        V = new vertex[Size];

        BOOL IsFirst = TRUE;
        INT k = 0;
        for (INT i = 0; i < RoadTris.size();)
        {
          if (IsFirst)
          {
            triangle T = RoadTris[i];

            V[k].P = PointsRoad[T.I1];
            V[k].T = vec2(0, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I2];
            V[k].T = vec2(0.5, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I3];
            V[k].T = vec2(0, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            i++;

            T = RoadTris[i];

            V[k].P = PointsRoad[T.I1];
            V[k].T = vec2(1, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I2];
            V[k].T = vec2(0.5, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I3];
            V[k].T = vec2(1, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            i++;

            T = RoadTris[i];

            V[k].P = PointsRoad[T.I1];
            V[k].T = vec2(1, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I2];
            V[k].T = vec2(0.5, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I3];
            V[k].T = vec2(0, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            i++;

            IsFirst = FALSE;
          }
          else
          {
            triangle T = RoadTris[i];

            V[k].P = PointsRoad[T.I1];
            V[k].T = vec2(0, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I2];
            V[k].T = vec2(0.5, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I3];
            V[k].T = vec2(0, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            i++;

            T = RoadTris[i];

            V[k].P = PointsRoad[T.I1];
            V[k].T = vec2(1, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I2];
            V[k].T = vec2(0.5, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I3];
            V[k].T = vec2(1, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            i++;

            T = RoadTris[i];

            V[k].P = PointsRoad[T.I1];
            V[k].T = vec2(1, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I2];
            V[k].T = vec2(0.5, 1);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            V[k].P = PointsRoad[T.I3];
            V[k].T = vec2(0, 0);
            V[k].N = vec3(0, 1, 0);
            V[k++].C = vec4(0);

            i++;

            if (i < RoadTris.size())
            {
              T = RoadTris[i];

              V[k].P = PointsRoad[T.I1];
              V[k].T = vec2(0, 0);
              V[k].N = vec3(0, 1, 0);
              V[k++].C = vec4(0);

              V[k].P = PointsRoad[T.I2];
              V[k].T = vec2(0.5, 1);
              V[k].N = vec3(0, 1, 0);
              V[k++].C = vec4(0);

              V[k].P = PointsRoad[T.I3];
              V[k].T = vec2(0, 1);
              V[k].N = vec3(0, 1, 0);
              V[k++].C = vec4(0);

              i++;

              T = RoadTris[i];

              V[k].P = PointsRoad[T.I1];
              V[k].T = vec2(0, 0);
              V[k].N = vec3(0, 1, 0);
              V[k++].C = vec4(0);

              V[k].P = PointsRoad[T.I2];
              V[k].T = vec2(0, 0);
              V[k].N = vec3(0, 1, 0);
              V[k++].C = vec4(0);

              V[k].P = PointsRoad[T.I3];
              V[k].T = vec2(0.5, 0);
              V[k].N = vec3(0, 1, 0);
              V[k++].C = vec4(0);

              i++;
            }
          }
        }

        INT* Ind;
        Ind = new INT[Size];

        for (INT i = 0; i < Size; i++)
          Ind[i] = i;

        //RD.PrimAutoNorm(V, Ind, Size);
        RD.PrimCreate(prim_type::PRIM_TRIMESH, V, Size, Ind, Size);

        RD.Mtl = &Mt2;
        IsRdCr = TRUE;

        delete[] Ind;
        delete[] V;
      }
    };

  /* Unit namespace */
  namespace units
  {
    vec3 RayApplyBebra(mth::ray<FLT> R, float T)
    {
      return R.Org + R.Dir * T;
    }

    bool PlaneIntersection(mth::ray<FLT> R, vec3 N, float D, vec3 *Res)
    {
      float nd = R.Dir & N;

      if (abs(nd) < 0.0001)
        return false;
      float T = (-(N & R.Org) - D) / nd;
      if (T < 0.0001)
        return false;

      if (Res != nullptr)
        *Res = RayApplyBebra(R, T);

      return true;
    }

    class ctrl : public unit
    {
    private:

      enum VIEW_MODE
      {
        TOP,
        FREE
      };

      vec3 Dir, Right, Up;
      points_pool PointsPool;
      prim Sphere, cdr, SphereInt, BbrSph, cdr2;

      vec3 SaveFreeCamLoc, SaveFreeCamAt, SaveFreeCamUp;
      vec3 SaveTopCamLoc = vec3(0, 100, 0), SaveTopCamAt = vec3(0, 0.000001, 0), SaveTopCamUp = vec3(0, 0, 1);

      VIEW_MODE View = TOP;
      MODE Mode = POLYGON_LINES;
      
      //sbuffer MamaIPapaSegmenti;

      material I = material("IntSph", vec3(0.0, 0.0, 0.0), vec3(0.01, 0.01, 0.01), vec3(0.5, 0.5, 0.5), 32, 1, "CITY");
      material I2 = material("BebrouSphere", vec3(0.1, 0.18725, 0.1745), vec3(0.396, 0.74151, 0.69102), vec3(0.297254, 0.30829, 0.306678), 12.8, 1, "CITY");
      material c3 = material("Bebroumama", vec3(0.8, 0.8, 0.8), vec3(0.396, 0.74151, 0.69102), vec3(0.297254, 0.30829, 0.306678), 12.8, 1, "MAMA");

      INT CurrPoint = 1;
      FLT tick = 0;
      BOOL Forward = TRUE;
    public:
      ctrl(VOID)
      {
        Sphere.PrimCreateSphere(0.6, vec3(0));
        Sphere.Mtl->Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/BRICK.g32", 4);
        Sphere.Mtl->Shd = anim::GetPtr()->shader_manager::Add("ARC170");
        //Sphere.Mtl->Shd = anim::GetPtr()->shader_manager::Add("DEFAULT");
        cdr.PrimCreateCylinder(0.4, 1, vec4(1), 30, 30);

        cdr2.PrimCreateCylinder(0.4, 1, vec4(1), 30, 30);
        cdr2.Mtl = &c3;
        cdr2.Mtl->Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/asdaqf.g32", 4);

        SphereInt.PrimCreateSphere(0.6, vec3(0));
        SphereInt.Mtl->Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/sad.g32", 4);;

        BbrSph.PrimCreateSphere(0.6, vec3(0));
        BbrSph.Mtl->Tex[0] = anim::GetPtr()->texture_manager::Add("BIN/TEXTURES/asdaqf.g32", 4);
        
        BbrSph.Mtl = &I2;

        //SphereInt.Mtl->Shd = anim::GetPtr()->shader_manager::Add("CITY");
        SphereInt.Mtl = &I;

        Dir = (vec3(0) - vec3(0, 10, 0)).Normalizing();
        Right = (Dir % vec3(0, 1, 0)).Normalizing();
        //MamaIPapaSegmenti.Create(6, PointsPool.Connections.data(), PointsPool.Connections.size() * sizeof(segment));

        //for (INT x = 0; x < 200; x += 2)
        //  for (INT z = 0; z < 200; z += 20)
        //    PointsPool.Add(vec3(x, 0, z), POINTS);
      }

      ~ctrl(VOID) override
      {
      }

      VOID Response(anim* Ani) override
      {
        static BOOL Saving;

        Dir = (Ani->Cam.At - Ani->Cam.Loc).Normalizing();
        Right = (Dir % vec3(0, 1, 0)).Normalizing();
        Up = (Right % Dir); /* Up */

        if (Ani->KeysClick['F'])
          Ani->FlipFullScreen();

        if (Ani->KeysClick['P'] && Mode != ROAD)
          PointsPool.CreatePointSetConvexHull2();

        if (Ani->KeysClick['H'])
          PointsPool.Loft();

        if (!Saving)
        {
          if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['T'])
          {
            if (View == FREE)
            {
              SaveFreeCamLoc = Ani->Cam.Loc;
              SaveFreeCamAt = Ani->Cam.At;
              SaveFreeCamUp = Ani->Cam.Up;

              Ani->Cam.SetLocAtUp(SaveTopCamLoc, SaveTopCamAt, SaveTopCamUp);
            }
            else
            {
              SaveTopCamLoc = Ani->Cam.Loc;
              SaveTopCamAt = Ani->Cam.At;
              SaveTopCamUp = Ani->Cam.Up;

              Ani->Cam.SetLocAtUp(SaveFreeCamLoc, SaveFreeCamAt, SaveFreeCamUp);
            }
          }


          FLT Time = Ani->DeltaTime * Ani->FPS / 30.0f;

          mth::vec3<FLT> Right = Ani->Cam.Right;
          Right[1] = 0;
          mth::vec3<FLT> Dir = Ani->Cam.Dir;
          Dir[1] = 0;

          if (Ani->KeysClick[27])
            exit(30);

          if (View != TOP)
          {
            if (Ani->Mx != 0 && Ani->Keys[VK_MBUTTON])
            {
              Ani->Cam.At -= Ani->Cam.Loc;
              Ani->Cam.At = mth::matr<FLT>::Rotate(Ani->Mdx / 30.0, Ani->Cam.Up).TransformVector(Ani->Cam.At);
              Ani->Cam.Right = mth::matr<FLT>::Rotate(Ani->Mdx / 30.0, Ani->Cam.Up).TransformVector(Ani->Cam.At);
              Ani->Cam.At += Ani->Cam.Loc;
              Ani->Cam.SetLocAtUp(Ani->Cam.Loc, Ani->Cam.At, Ani->Cam.Up);
            }
            if (Ani->Mdy != 0 && Ani->Keys[VK_MBUTTON])
            {
              Ani->Cam.At -= Ani->Cam.Loc;
              Ani->Cam.At = mth::matr<FLT>::Rotate(Ani->Mdy / 30.0, Ani->Cam.Right).TransformVector(Ani->Cam.At);
              Ani->Cam.At += Ani->Cam.Loc;
              Ani->Cam.Up = (Right % Dir).Normalizing();
              Ani->Cam.SetLocAtUp(Ani->Cam.Loc, Ani->Cam.At, Ani->Cam.Up);
            }
          }

          if (Ani->Keys[VK_SHIFT] && Ani->Keys[VK_LCONTROL] && Ani->KeysClick['S'])
          {
            Saving = TRUE;
            PointsPool.SaveFile();
          }

          if (Ani->Keys[VK_SHIFT] && Ani->Keys[VK_LCONTROL] && Ani->KeysClick['O'])
          {
            PointsPool.LoadFile();
          }

          if (Ani->KeysClick[VK_DELETE])
            PointsPool.FullClear();

          if (Ani->Keys[VK_RBUTTON])
          {
            FLT Hp, Wp = Hp = Ani->Cam.ProjDist;

            if (Ani->Cam.FrameW > Ani->Cam.FrameH)
              Wp *= static_cast<FLT>(Ani->Cam.FrameW) / Ani->Cam.FrameH;
            else
              Hp *= static_cast<FLT>(Ani->Cam.FrameH) / Ani->Cam.FrameW;

            FLT Dist = !(Ani->Cam.At - Ani->Cam.Loc);

            FLT sx = -Ani->Mdx * Wp / Ani->Cam.FrameW * Dist / Ani->Cam.ProjDist;
            FLT sy = Ani->Mdy * Hp / Ani->Cam.FrameH * Dist / Ani->Cam.ProjDist;

            Ani->Cam.Move((Ani->Cam.Right * sx * 3 + Ani->Cam.Up * sy * 3));
          }
          if (Ani->Keys['W'])
            Ani->Cam.Move(Dir * Time * 1);
          if (Ani->Keys['S'])
            Ani->Cam.Move(-Dir * Time * 1);
          if (Ani->Keys['D'])
            Ani->Cam.Move(Right * Time * 30);
          if (Ani->Keys['A'])
            Ani->Cam.Move(-Right * Time * 30);
          if (Ani->Keys[VK_SPACE])
            Ani->Cam.Move(vec3(0, Time, 0) * 100);
          if (Ani->Keys[VK_SHIFT])
            Ani->Cam.Move(vec3(0, -Time, 0) * 100);
          Ani->Cam.Move(Dir * Ani->Mdz * Time / 10);

          if (Ani->KeysClick['M'])
          {
            PointsPool.CreateConvexHull();
            //MamaIPapaSegmenti.Update(PointsPool.Connections.data(), PointsPool.Connections.size() * sizeof(segment));
          }

          if (Ani->KeysClick['U'])
            Ani->shader_manager::UpdateShd();

          if (Ani->KeysClick[VK_LBUTTON])
          {
            POINT pt;

            GetCursorPos(&pt);

            vec3 P;
            Ani->Cam.SetRay(pt.x, pt.y);
            if (PlaneIntersection(Ani->Cam.Ray, vec3(0, 1, 0), 0, &P))
            {
              PointsPool.Add(P, Mode);
              PointsPool.CheckForInterSections();
              //MamaIPapaSegmenti.Update(PointsPool.Connections.data(), PointsPool.Connections.size() * sizeof(segment));
            }
          }

          if (Ani->KeysClick['L'])
          {
            PointsPool.Triangulate();
            //MamaIPapaSegmenti.Update(PointsPool.Connections.data(), PointsPool.Connections.size() * sizeof(segment));
          }

          if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['Z'])
          {
            PointsPool.PopLastPoint(Mode);
            PointsPool.CheckForInterSections();
           // MamaIPapaSegmenti.Update(PointsPool.Connections.data(), PointsPool.Connections.size() * sizeof(segment));
          }

          if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick[VK_MBUTTON])
          {
            POINT pt;

            GetCursorPos(&pt);

            Ani->Cam.SetRay(pt.x, pt.y);

            vec3 P;

            if (PlaneIntersection(Ani->Cam.Ray, vec3(0, 1, 0), 0, &P))
              PointsPool.AddPointToHeightSet(P);
          }

          if (Ani->Keys[VK_LCONTROL] && Ani->Mdz != 0)
            PointsPool.AddToCurrentHeightPoints(Ani->Mdz / 100);

          if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['C'])
            PointsPool.ClearHeightSet();

          if (Ani->KeysClick['B'])
            PointsPool.CountBisectorsForEveryAngle(PointsPool.Polygons[0]);

          if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['1'])
            Mode = POINTS;
          else if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['2'])
            Mode = SEGMENTS;
          else if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['3'])
            Mode = POLYGON_LINES;
          else if (Ani->Keys[VK_LCONTROL] && Ani->KeysClick['4'])
            Mode = ROAD;

          if (Ani->KeysClick['5'])
            PointsPool.CreatePrimitiveFromPP();

          if (Ani->KeysClick['6'])
            PointsPool.CreateRoadPrim();

        }
      }

      VOID Render(anim* Ani) override
      {
        if (!PointsPool.IsPrimCreated)
        {
          for (INT i = 0; i < PointsPool.GetSize(); i++)
          {
            if (!PointsPool.Find(i, nullptr))
              Ani->PrimDraw(&Sphere, matr::Translate(PointsPool[i]));
            else
              Ani->PrimDraw(&SphereInt, matr::Translate(PointsPool[i]));
          }

          for (INT i = 0; i < PointsPool.GetIntSize(); i++)
            Ani->PrimDraw(&SphereInt, matr::Translate(PointsPool(i)));

          if (PointsPool.Connections.size() > 0)
          {
            // MamaIPapaSegmenti.Apply();
            for (INT i = 0; i < PointsPool.Connections.size(); i++)
            {
              vec3 P1 = PointsPool.PointsGet(PointsPool.Connections[i].St), P2 = PointsPool.PointsGet(PointsPool.Connections[i].End);
              vec3 P = P2 - P1;
              FLT L = sqrt(P[0] * P[0] + P[2] * P[2] + P[1] * P[1]);

              FLT a = (atan2(P[0], P[2])) * 180 / PI, b = ((atan2(P[1], P[2])) * 180 / PI), c = ((atan2(P[1], P[0])) * 180 / PI);

              if (fabs(P.Y) < 0.001)
                Ani->PrimDraw(&cdr, matr::Scale(vec3(1, L, 1)) * matr::RotateZ(90) * matr::RotateY(a - 90) * matr::Translate(P2)); //  * matr::RotateX(180 - b) 
              //else
              //Ani->PrimDraw(&cdr, matr::Scale(vec3(1, L, 1)) * matr::RotateZ(c + 90) * matr::RotateY(a - 90) * matr::RotateX(90 + b) * matr::Translate(P2)); //  * matr::RotateX(180 - b) 
            }
          }

          /*if (PointsPool.RoadSegs.size() > 0)
            for (INT i = 0; i < PointsPool.RoadSegs.size(); i++)
            {
              //vec3 P1 = PointsPool.PointsGet(PointsPool.Connections[i].St), P2 = PointsPool.PointsGet(PointsPool.Connections[i].End);
              vec3 P1 = PointsPool.PointsRoad[PointsPool.RoadSegs[i].St], P2 = PointsPool.PointsRoad[PointsPool.RoadSegs[i].End];
              vec3 P = P2 - P1;
              FLT L = sqrt(P[0] * P[0] + P[2] * P[2] + P[1] * P[1]);

              FLT a = (atan2(P[0], P[2])) * 180 / PI, b = ((atan2(P[1], P[2])) * 180 / PI), c = ((atan2(P[1], P[0])) * 180 / PI);
              Ani->PrimDraw(&cdr, matr::Scale(vec3(1, L, 1)) * matr::RotateZ(90) * matr::RotateY(a - 90) * matr::Translate(P2)); //  * matr::RotateX(180 - b) 
            }*/
        }
        else if (PointsPool.IsPrimCreated)
          Ani->PrimDraw(&PointsPool.PP, matr::Identity());

        if (!PointsPool.IsRdCr)
        {
          if (PointsPool.RoadConnections.size() > 0)
          {
            // MamaIPapaSegmenti.Apply();
            for (INT i = 0; i < PointsPool.RoadConnections.size(); i++)
            {
              vec3 P1 = PointsPool.PointsPolygon[PointsPool.RoadConnections[i].St], P2 = PointsPool.PointsPolygon[PointsPool.RoadConnections[i].End];
              vec3 P = P2 - P1;
              FLT L = sqrt(P[0] * P[0] + P[2] * P[2] + P[1] * P[1]);

              FLT a = (atan2(P[0], P[2])) * 180 / PI, b = ((atan2(P[1], P[2])) * 180 / PI), c = ((atan2(P[1], P[0])) * 180 / PI);

              Ani->PrimDraw(&cdr, matr::Scale(vec3(1, L, 1)) * matr::RotateZ(90) * matr::RotateY(a - 90) * matr::Translate(P2));
            }
          }

          for (INT i = 0; i < PointsPool.PointsPolygon.size(); i++)
            Ani->PrimDraw(&SphereInt, matr::Translate(PointsPool.PointsPolygon[i]));

          if (PointsPool.RoadSegs.size() > 0)
            for (INT i = 0; i < PointsPool.RoadSegs.size(); i++)
            {
              //vec3 P1 = PointsPool.PointsGet(PointsPool.Connections[i].St), P2 = PointsPool.PointsGet(PointsPool.Connections[i].End);
              vec3 P1 = PointsPool.PointsRoad[PointsPool.RoadSegs[i].St], P2 = PointsPool.PointsRoad[PointsPool.RoadSegs[i].End];
              vec3 P = P2 - P1;
              FLT L = sqrt(P[0] * P[0] + P[2] * P[2] + P[1] * P[1]);

              FLT a = (atan2(P[0], P[2])) * 180 / PI, b = ((atan2(P[1], P[2])) * 180 / PI), c = ((atan2(P[1], P[0])) * 180 / PI);
              Ani->PrimDraw(&cdr, matr::Scale(vec3(1, L, 1)) * matr::RotateZ(90) * matr::RotateY(a - 90) * matr::Translate(P2)); //  * matr::RotateX(180 - b) 
            }
        }
        else
        {
          if (PointsPool.RoadConnections.size() > 0)
          {
            // MamaIPapaSegmenti.Apply();
            for (INT i = 0; i < PointsPool.RoadConnections.size(); i++)
            {
              vec3 P1 = PointsPool.PointsPolygon[PointsPool.RoadConnections[i].St], P2 = PointsPool.PointsPolygon[PointsPool.RoadConnections[i].End];
              vec3 P = P2 - P1;
              FLT L = sqrt(P[0] * P[0] + P[2] * P[2] + P[1] * P[1]);

              FLT a = (atan2(P[0], P[2])) * 180 / PI, b = ((atan2(P[1], P[2])) * 180 / PI), c = ((atan2(P[1], P[0])) * 180 / PI);

              if (L > 3)
              {
                FLT NewL = 3;
                INT n = (INT)(L / NewL);

                for (INT i = 0; i < n; i += 2)
                {
                  P.Normalize();
                  vec3 PTotrans = P2 - (P * i * NewL + P);

                  Ani->PrimDraw(&cdr2, matr::Scale(vec3(1, NewL, 1)) * matr::RotateZ(90) * matr::RotateY(a - 90) * matr::Translate(PTotrans));
                }
              }
              
            }

            vec3 P1 = PointsPool.PointsPolygon[CurrPoint] - PointsPool.PointsPolygon[CurrPoint - 1];
            vec3 PTT;
           
            if (Forward)
              PTT = PointsPool.PointsPolygon[CurrPoint] - (P1 * (1 - tick));
            else
              PTT = PointsPool.PointsPolygon[CurrPoint] - (P1 * (tick));

            //FLT Time = Ani->DeltaTime * Ani->FPS / 0.5f;
            vec3 D = P1.Normalizing();

            FLT A = atan2(D.Z - PTT.Z, D.X - PTT.X) * 180 / PI;

            Ani->PrimDraw(&PointsPool.RD, matr::Translate(vec3(0, 0.05, 0)));
            Ani->PrimDraw(&PointsPool.rdSph, matr::RotateX(Ani->GlobalTime * 300) * matr::RotateY(A) * matr::Translate(PTT + vec3(0, 0.5, 0)));

            tick += Ani->GlobalDeltaTime * 30;

            if (tick >= 1)
            {
              tick = 0;
              if (Forward)
                CurrPoint++;
              else
                CurrPoint--;

              if (CurrPoint >= PointsPool.PointsPolygon.size())
                CurrPoint--, Forward = FALSE;
              else if (CurrPoint <= 0)
                CurrPoint++, Forward = TRUE;
            }
          }
          
        }
      }
    };

    class plane : public unit
    {
    private:
      vertex V[4];
      vec3 N = vec3(0, 1, 0);
      const FLT PlaneSize = 100000;
      prim Fl;
      material A = material("PlaneBebrou", vec3(0.0, 0.0, 0.0), vec3(0.01, 0.01, 0.01), vec3(0.5, 0.5, 0.5), 32, 1, "DEFAULT");
      BOOL DrawFlag = TRUE;

    public:
      plane(VOID)
      {
        V[0].P = vec3(-PlaneSize, 0, PlaneSize);
        V[0].T = vec2(0, 1);
        V[0].N = N;
        V[0].C = vec4(0);

        V[1].P = vec3(-PlaneSize, 0, -PlaneSize);
        V[1].T = vec2(0, 0);
        V[1].N = N;
        V[1].C = vec4(0);

        V[2].P = vec3(PlaneSize, 0, -PlaneSize);
        V[2].T = vec2(1, 0);
        V[2].N = N;
        V[2].C = vec4(0);

        V[3].P = vec3(PlaneSize, 0, PlaneSize);
        V[3].T = vec2(1, 1);
        V[3].N = N;
        V[3].C = vec4(0);

        INT Ind[6] = {0, 1, 2, 0, 2, 3};
        
        Fl.PrimCreate(prim_type::PRIM_TRIMESH, V, 4, Ind, 6);

        Fl.Mtl = &A;
      }

      ~plane(VOID) override
      {
      }

      VOID Response(anim* Ani) override
      {
        if (Ani->KeysClick['J'])
          DrawFlag = !DrawFlag;
      }

      VOID Render(anim* Ani) override
      {
        if (DrawFlag)
          Ani->PrimDraw(&Fl, matr::Identity());
      }
    };

  } /* End of 'units' namespace */
} /* End of 'nicg' namespace */

#endif /* __units_h_ */

/* END OF 'units.h' FILE */