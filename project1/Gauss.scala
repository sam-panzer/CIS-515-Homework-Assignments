object Gauss {
    type Matrix = Array[Array[Double]]
    def maxOf(l: Seq[Double]): Int = {
        val m = l.zipWithIndex
        m.reduceLeft((acc,x) => {
            val (p,i) = x
            val (ap, ai) = acc
            if (math.abs(p) > math.abs(ap)) x else acc
        })._2
    }
    def swapRows(mat: Matrix, a: Int, b: Int) = {
        val start = math.min(a,b)
        val end = math.max(a,b)
        val s = mat(start)
        mat(start) = mat(end)
        mat(end) = s
    }
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
    def printMatrix(mat: Matrix) = {
        for (i <- 0 until mat.length) {
            print("[ ")
            for (j <- 0 until mat(i).length) {
                print(mat(i)(j) + " \t")
            }
            println(" ]")
        }
    }
    def backSubstitute(mat: Matrix) = {
        val m = mat.clone()
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
