; ModuleID = 'Test.c'
source_filename = "Test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @test() #0 {
entry:
  %p = alloca i32, align 4
  %q = alloca i32, align 4
  %r = alloca i32, align 4
  %s = alloca i32, align 4
  %u = alloca i32, align 4
  %v = alloca i32, align 4
  br label %while.body

while.body:                                       ; preds = %entry, %if.end
  %0 = load i32, i32* %q, align 4
  %1 = load i32, i32* %r, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, i32* %p, align 4
  %2 = load i32, i32* %p, align 4
  %3 = load i32, i32* %q, align 4
  %add1 = add nsw i32 %2, %3
  store i32 %add1, i32* %s, align 4
  %4 = load i32, i32* %s, align 4
  %5 = load i32, i32* %v, align 4
  %mul = mul nsw i32 %4, %5
  store i32 %mul, i32* %u, align 4
  %6 = load i32, i32* %v, align 4
  %cmp = icmp sgt i32 %6, 0
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %while.body
  %7 = load i32, i32* %r, align 4
  %8 = load i32, i32* %u, align 4
  %add2 = add nsw i32 %7, %8
  store i32 %add2, i32* %v, align 4
  br label %if.end

if.else:                                          ; preds = %while.body
  %9 = load i32, i32* %s, align 4
  %10 = load i32, i32* %u, align 4
  %mul3 = mul nsw i32 %9, %10
  store i32 %mul3, i32* %q, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %11 = load i32, i32* %v, align 4
  %12 = load i32, i32* %r, align 4
  %add4 = add nsw i32 %11, %12
  store i32 %add4, i32* %q, align 4
  br label %while.body
}

attributes #0 = { noinline nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 12.0.1"}
