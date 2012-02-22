object Gauss {
    type Matrix = Array[Array[Double]]

    // Determines the index of the highest value
    def maxOf(l: Seq[Double]): Int = {
        val m = l.zipWithIndex
        m.reduceLeft((acc,x) => {
            val (p,i) = x
            val (ap, ai) = acc
            if (math.abs(p) > math.abs(ap)) x else acc
        })._2
    }

    // Swaps rows a and b in matrix mat
    def swapRows(mat: Matrix, a: Int, b: Int) = {
        val start = math.min(a,b)
        val end = math.max(a,b)
        val s = mat(start)
        mat(start) = mat(end)
        mat(end) = s
    }

    // Gaussian elimination
    def elimination(mat: Matrix): Matrix = {
        val m = mat.clone
        for (c <- 0 until m.length) {
            val col = m.map(x => x.apply(c))
            val iMax = c + maxOf(col.drop(c))
            val vMax = col(iMax)
            if (vMax != 0) {
                swapRows(m, c, iMax)
              for (i <- (c + 1) until m.length) {
                  for (j <- (c  + 1) until m(i).length) {
                      m(i)(j) -= m(c)(j) * (m(i)(c) / m(c)(c))
                  }
                  m(i)(c) = 0
              }
            }
        }
        m
    }

    // Prints the matrix mat. Not the prettiest printer, but better than just
    // writing to stdout
    def printMatrix(mat: Matrix) = {
        for (i <- 0 until mat.length) {
            print("[ ")
            for (j <- 0 until mat(i).length) {
                print(mat(i)(j) + " \t")
            }
            println(" ]")
        }
    }

    // Assumes that mat is a triangular matrix with a nonzero trace
    def backSubstitute(mat: Matrix, dest: List[Double]): List[Double] = {
        def dot(v1: Seq[Double], v2: Seq[Double]) = {
          val w: Seq[(Double, Double)] = v1 zip v2
          w.foldLeft (0.0) ((acc: Double, x: (Double,Double)) => {
            val (p1, p2) = x
            acc + p1 * p2
          }) 
        }
        mat.foldRight (Nil: List[Double])((x: Array[Double],acc: List[Double]) =>{
            val w = x.takeRight(acc.length + 1)  
            ((dest.drop(acc.length)(0) - dot(w.drop(1), acc)) / w(0)) :: acc
          })
    }
}

object Main {
    def main(args: Array[String]) = {
        val mat = Array(Array(1.0,2.0,3.0,4.0),
                        Array(5.0,6.0,8.0,8.0),
                        Array(7.0,2.0,5.0,1.0),
                        Array(1.0,2.0,0.0,6.0))
        Gauss.printMatrix(mat)
        val m = Gauss.elimination(mat)
        println()
        Gauss.printMatrix(mat)
    }
}
