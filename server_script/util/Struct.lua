module('Struct', package.seeall)

--双端队列
deque = class()

function deque.__init__(self)
   self:reset()
end

function deque.reset(self)
   self.first = 0
   self.last = -1

   self.tData = {}
end

function deque.pushFront(self,value)
   self.first = self.first-1
   self.tData[ self.first ]=value
end

function deque.pushBack(self,value)
   self.last=self.last+1
   self.tData[self.last] = value
end

function deque.popFront(self)
   local first = self.first
   if first > self.last then 
      error("List is empty!")
   end
   local value = self.tData[first]
   self.tData[first] = nil
   self.first = first+1
   return value
end

function deque.popBack(self)
   local last = self.last
   if last < self.first then 
      error("List is empty!")
   end
   local value = self.tData[last]
   self.tData[last] = nil
   self.last = last-1
   return value
end



function test_deque()
   -- try
   --    print("======test_deque====")
   -- catch err do
   --    print("======test_deque=catch===")
   -- end
   lp = deque()
   lp:pushFront(1)
   lp:pushFront(2)
   lp:pushBack(-1)
   lp:pushBack(-2)
   x = lp:popFront(lp)
   print("test_deque x = " .. x)
   x = lp:popBack(lp)
   print("test_deque x = " .. x)
   x = lp:popFront(lp)
   print("test_deque x = " .. x)
   x = lp:popBack(lp)
   print("test_deque x = " .. x)
   
   --x=lp:popBack(lp)
   --print(x)
end
--test_deque()

--输出结果
-- 2
-- -2
-- 1
-- -1
-- lua：... List is empty！

--==========================================





