import quicksort

class Submission(quicksort.QSortBase):
    def pivot(self, lo, hi, depth, breadth):
        a, b, c = self.array[lo], self.array[(lo+hi)>>1], self.array[hi]
        if a > b:
            if b > c:
                return b
            return c if a > c else a
        return a if a > c else c if b > c else b
