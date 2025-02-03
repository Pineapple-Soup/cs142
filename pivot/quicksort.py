import dis
import types

from sys import settrace,setprofile,_getframe

# For your testing, you may wish to allow these
INVALID = ('print','sort','sorted','exec','eval')

class OpCount:
    def __init__(self):
        self.count = 0
        return
    def __call__(self, frame, kind, arg):
        if kind == 'opcode':
            self.count += 1
        elif kind == 'c_call':
            if arg.__name__ in INVALID:
                raise RuntimeError("You are not allowed to call %r"%arg.__name__)
        elif kind == 'call':
            if frame.f_code.co_name in INVALID:
                raise RuntimeError("You are not allowed to call %r"%frame.f_code.co_name)
        frame.f_trace_opcodes = True
        return self
    def __enter__(self):
        frame = _getframe()
        frame.f_trace_opcodes = True
        setprofile(self)
        settrace(self)
        return self
    def __exit__(self,*args):
        settrace(None)
        setprofile(None)
        return

class QSortBase:
    def __init__(self, array):
        self.array = array
        self.levels = []
        self.count = 0
        self.qsort(0,len(array)-1,0)
        return

    def pivot(self, lo, hi):
        raise NotImplementedError

    def qpartition(self, lo,hi,pivot):
        while 1:
            while self.array[lo] < pivot:
                lo += 1
            while pivot < self.array[hi]:
                hi -= 1
            if lo >= hi: break

            self.array[lo],self.array[hi] = self.array[hi],self.array[lo] 
            lo += 1
            hi -= 1
        return hi
    

    def qsort(self,lo,hi,depth):
        while len(self.levels) <= depth: self.levels.append([])

        # Base case
        if hi-lo < 2:
            # This meant I had a partition of size 1 or less and
            # did not pivot
            self.levels[depth].append(None)
            return
        
        # Pick a pivot.  We let the pivot guesser know
        # some stuff about what we're doing
        breadth = len(self.levels[depth])
        with OpCount() as counter:
            pivot = self.pivot(lo,hi,depth,breadth)
        self.count += counter.count
        self.levels[depth].append(pivot)

        split = self.qpartition(lo,hi,pivot)
        self.qsort(lo,split,depth+1)
        self.qsort(split+1,hi,depth+1)
        return

if __name__ == '__main__':
    from pivot import Submission

    Q = Submission([33,55,77,22,88])
    print(Q.array)
    print(Q.levels)
    print(Q.count)