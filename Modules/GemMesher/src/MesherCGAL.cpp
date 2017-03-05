#include "MesherCGAL.h"

#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>
// IO
#include <CGAL/IO/Polyhedron_iostream.h>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3 <K> Polyhedron;
typedef CGAL::Polyhedral_mesh_domain_3 <Polyhedron, K> Mesh_domain;
// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3 <Tr> C3t3;
// Criteria
typedef CGAL::Mesh_criteria_3 <Tr> Mesh_criteria;
// To avoid verbose function and named parameters call

using namespace gem;
using namespace CGAL::parameters;
using namespace std;

namespace
{
    void cgalMesh(float size, float ratio, std::istream &in, std::ostream &out)
    {
        // Create input polyhedron
        Polyhedron polyhedron;
        in >> polyhedron;
        if (in.bad())
        {
            std::cerr << "Error: Cannot read input" << std::endl;
            return;
        }

        // Create domain
        Mesh_domain domain(polyhedron);

        // Mesh criteria (no cell_size set)
        Mesh_criteria criteria(cell_size= size, cell_radius_edge_ratio= ratio);

        // Mesh generation
        C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, odt());

        // Output
        c3t3.output_to_medit(out);
    }
}

void MesherCGAL::compute(void)
{
    auto surface = m_spSurface;

    // Generate an OFF stringstream
    stringstream off;
    off << "OFF" << endl;
    off << surface->GetNumberOfPoints() << " " << surface->GetNumberOfCells() << " " << 0 << endl;
    for (int i = 0; i < surface->GetNumberOfPoints(); i++)
    {
        double p[3];
        surface->GetPoint(i, p);
        off << p[0] << " " << p[1] << " " << p[2] << endl;
    }
    surface->BuildCells();
    for (int i = 0; i < surface->GetNumberOfCells(); i++)
    {
        vtkIdType npts, *pts;
        surface->GetCellPoints(i, npts, pts);
        off << npts;
        for (int j = 0; j < npts; j++)
            off << " " << pts[j];
        off << " " << 0 << endl;
    }

    // Generate tetrahedron mesh
    stringstream medit;
    cgalMesh(m_options.fEdgeSize, m_options.fRadiusEdgeRatio, off, medit);

    // Convert from medit format to vtu
    int nvert, ncell, ix;
    double x, y, z;
    string line;

    do
    {
        getline(medit, line);
    }
    while (line.compare("Vertices"));
    medit >> nvert;
    auto pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetNumberOfPoints(nvert);
    for (int i = 0; i < nvert; i++)
    {
        medit >> x >> y >> z >> ix;
        pts->SetPoint(i, x, y, z);
    }

    auto mesh = m_spUGrid;
    mesh->SetPoints(pts);
    mesh->Allocate();

    do
    {
        getline(medit, line);
    }
    while (line.compare("Tetrahedra"));
    medit >> ncell;
    for (int i = 0; i < ncell; i++)
    {
        vtkIdType ids[4];
        medit >> ids[0] >> ids[1] >> ids[2] >> ids[3] >> ix;
        for (int j = 0; j < 4; j++)
            ids[j]--;
        mesh->InsertNextCell(VTK_TETRA, 4, ids);
    }
}