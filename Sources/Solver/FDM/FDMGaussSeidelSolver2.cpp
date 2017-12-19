/*************************************************************************
> File Name: FDMGaussSeidelSolver2.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 2-D finite difference-type linear system solver using Gauss-Seidel method.
> Created Time: 2017/08/17
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Solver/FDM/FDMGaussSeidelSolver2.h>

namespace CubbyFlow
{
	FDMGaussSeidelSolver2::FDMGaussSeidelSolver2(
		unsigned int maxNumberOfIterations,
		unsigned int residualCheckInterval,
		double tolerance,
		double sorFactor,
		bool useRedBlackOrdering) :
		m_maxNumberOfIterations(maxNumberOfIterations),
		m_lastNumberOfIterations(0),
		m_residualCheckInterval(residualCheckInterval),
		m_tolerance(tolerance),
		m_lastResidual(std::numeric_limits<double>::max()),
		m_sorFactor(sorFactor),
		m_useRedBlackOrdering(useRedBlackOrdering)
	{
		// Do nothing
	}

	bool FDMGaussSeidelSolver2::Solve(FDMLinearSystem2* system)
	{
		m_residual.Resize(system->x.size());

		m_lastNumberOfIterations = m_maxNumberOfIterations;

		for (unsigned int iter = 0; iter < m_maxNumberOfIterations; ++iter)
		{
			if (m_useRedBlackOrdering)
			{
				RelaxRedBlack(system->A, system->b, m_sorFactor, &system->x);
			}
			else
			{
				Relax(system->A, system->b, m_sorFactor, &system->x);
			}

			if (iter != 0 && iter % m_residualCheckInterval == 0)
			{
				FDMBLAS2::Residual(system->A, system->x, system->b, &m_residual);

				if (FDMBLAS2::L2Norm(m_residual) < m_tolerance)
				{
					m_lastNumberOfIterations = iter + 1;
					break;
				}
			}
		}

		FDMBLAS2::Residual(system->A, system->x, system->b, &m_residual);
		m_lastResidual = FDMBLAS2::L2Norm(m_residual);

		return m_lastResidual < m_tolerance;
	}

	unsigned int FDMGaussSeidelSolver2::GetMaxNumberOfIterations() const
	{
		return m_maxNumberOfIterations;
	}

	unsigned int FDMGaussSeidelSolver2::GetLastNumberOfIterations() const
	{
		return m_lastNumberOfIterations;
	}

	double FDMGaussSeidelSolver2::GetTolerance() const
	{
		return m_tolerance;
	}

	double FDMGaussSeidelSolver2::GetLastResidual() const
	{
		return m_lastResidual;
	}

	double FDMGaussSeidelSolver2::GetSORFactor() const
	{
		return m_sorFactor;
	}

	bool FDMGaussSeidelSolver2::GetUseRedBlackOrdering() const
	{
		return m_useRedBlackOrdering;
	}

	void FDMGaussSeidelSolver2::Relax(const FDMMatrix2& A, const FDMVector2& b,
		double sorFactor, FDMVector2* x)
	{
		Size2 size = A.size();
		FDMVector2& refX = *x;

		A.ForEachIndex([&](size_t i, size_t j)
		{
			double r =
				((i > 0) ? A(i - 1, j).right * refX(i - 1, j) : 0.0) +
				((i + 1 < size.x) ? A(i, j).right * refX(i + 1, j) : 0.0) +
				((j > 0) ? A(i, j - 1).up * refX(i, j - 1) : 0.0) +
				((j + 1 < size.y) ? A(i, j).up * refX(i, j + 1) : 0.0);

			refX(i, j) = (1.0 - sorFactor) * refX(i, j) +
				sorFactor * (b(i, j) - r) / A(i, j).center;
		});
	}

	void FDMGaussSeidelSolver2::RelaxRedBlack(const FDMMatrix2& A, const FDMVector2& b,
		double sorFactor, FDMVector2* x)
	{
		Size2 size = A.size();
		FDMVector2& refX = *x;

		// Red update
		ParallelRangeFor(
			ZERO_SIZE, size.x, ZERO_SIZE, size.y,
			[&](size_t iBegin, size_t iEnd, size_t jBegin, size_t jEnd)
		{
			for (size_t j = jBegin; j < jEnd; ++j)
			{
				// i.e. (0, 0)
				size_t i = j % 2 + iBegin;
				
				for (; i < iEnd; i += 2)
				{
					double r =
						((i > 0) ? A(i - 1, j).right * refX(i - 1, j) : 0.0) +
						((i + 1 < size.x) ? A(i, j).right * refX(i + 1, j) : 0.0) +
						((j > 0) ? A(i, j - 1).up * refX(i, j - 1) : 0.0) +
						((j + 1 < size.y) ? A(i, j).up * refX(i, j + 1) : 0.0);

					refX(i, j) = (1.0 - sorFactor) * refX(i, j) +
						sorFactor * (b(i, j) - r) / A(i, j).center;
				}
			}
		});

		// Black update
		ParallelRangeFor(
			ZERO_SIZE, size.x, ZERO_SIZE, size.y,
			[&](size_t iBegin, size_t iEnd, size_t jBegin, size_t jEnd)
		{
			for (size_t j = jBegin; j < jEnd; ++j)
			{
				size_t i = 1 - j % 2 + iBegin;  // i.e. (1, 0)
				
				for (; i < iEnd; i += 2)
				{
					double r =
						((i > 0) ? A(i - 1, j).right * refX(i - 1, j) : 0.0) +
						((i + 1 < size.x) ? A(i, j).right * refX(i + 1, j) : 0.0) +
						((j > 0) ? A(i, j - 1).up * refX(i, j - 1) : 0.0) +
						((j + 1 < size.y) ? A(i, j).up * refX(i, j + 1) : 0.0);

					refX(i, j) = (1.0 - sorFactor) * refX(i, j) +
						sorFactor * (b(i, j) - r) / A(i, j).center;
				}
			}
		});
	}
}