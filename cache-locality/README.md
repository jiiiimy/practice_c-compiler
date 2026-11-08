# T01-03 CPU Cache Locality Experiment

## 目的・仮説
同一配列(`uint32_t` × 1,000,000 = 4,000,000 byte)の全要素を1回ずつ読むとき、
順次アクセス(A)はstride=16アクセス(B)より空間的局所性を使いやすく速い可能性がある。
結果が予測と違っても測定値をそのまま報告する。

## 実装 (`cache_bench.c`)
- A: `for i in 0..N: sum += data[i]`
- B: `for offset in 0..16: for i = offset; i < N; i += 16: sum += data[i]` (全要素を重複なく1回)
- 配列確保・初期化・期待checksum計算は計測外。両カーネルとも `noinline`、同一データ。
- 1サンプル = 50走査(=5,000万アクセス)をまとめて計測。Warm-up 5サンプル(破棄)後、31サンプル。
  A/Bの実行順は毎サンプル交互(AB, BA, ...)。
- 時計: `clock_gettime(CLOCK_MONOTONIC)`。
- Checksum: 独立ループで求めた期待値×50と全サンプルで照合(不一致なら exit 2)。

## ビルド・実行
```
make            # bench_O2 (-O2) と bench_novec (-O2 -fno-vectorize -fno-slp-vectorize -fno-unroll-loops)
make run        # 両方実行
make asm        # asm_O2.s / asm_novec.s を出力(最適化でロードが消えていないか確認用)
```

## 結果の見方
出力の `median` / `ns/access` / min / max を比較する。`checksum_all_match=yes` を必ず確認。
`bench_O2` と `bench_novec` の差はベクトル化・アンロールの影響。生データは `results/`、考察は `REPORT.md`。

## 限界
- 4MBはこのCPUのL2(P-core cluster 12MB)に収まるため、L2以下の差を見ており、DRAMアクセスの差ではない。
- 時間差からCache Miss差は断定できない(性能カウンタ未使用)。
- 単一マシン・単一スレッド、OSスケジューリング/周波数の影響は未制御。
- 詳細は `REPORT.md`。
