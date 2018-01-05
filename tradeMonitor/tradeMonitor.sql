--1.未及时得到交易所确认的纪录监控
type=1
sql=select wth, khh, gdh, jys, zqdm, wtlb, sbjg, wtsj from securities.tDRWT where SBJG<2 and
  sign(to_char(sysdate, 'hh24miss')-'093000')>=0 and
  sign(to_char(sysdate, 'hh24miss')-'150000')<=0 and
  ROUND(TO_NUMBER(to_date(to_char(sysdate, 'HH24:mi:ss'),'hh24:mi:ss') - to_date(WTSJ, 'hh24:mi:ss'))*24*60*60)>5

--2.帐户出现交易后可能透支的风险监控
type=1
sql=select A.KHH, A.KHXM, A.ZJZH, A.BZ, A.DJJE, A.ZHYE from account.tZJZH A, securities.tDRWT B where A.KHH=B.KHH and A.DJJE>A.ZHYE and A.ZHYE>0

--3.特殊业务发生监控，及时提醒对当日这些业务的最终确认状态进行确认
type=1
sql=select wth, khh, gdh, jys, zqdm, wtlb, sbjg, wtsj from securities.tDRWT where SBJG<3 and ((regexp_like(ZQLB,'[FHT].+') or ZQLB in('A8','A9','AB','BB'))) and 
  ZQLB<>'T0'

--4.大宗交易委托情况提醒，委托后5-10分钟未成交，或者临收盘5-10分钟还未成交等等
type=1
sql=select wth, khh, gdh, jys, zqdm, wtlb, sbjg, wtsj from securities.tdrwt where cjsl=0 and wtlb in (57,58,59,60) and
  sign(to_char(sysdate, 'hh24miss')-'150000')>0 and sign(to_char(sysdate, 'hh24miss')-'153000')<=0 and
  ROUND(TO_NUMBER(to_date(to_char(sysdate,'HH24:mi:ss'),'hh24:mi:ss') - to_date(wtsj,'hh24:mi:ss'))*24*60*60)>120 and
  ROUND(TO_NUMBER(to_date(to_char(sysdate,'HH24:mi:ss'),'hh24:mi:ss') - to_date('15:01:00','hh24:mi:ss'))*24*60*60)>0

--5.清算后持仓成本变化、盈亏变化是否正确
type=2
sql=select khh, khxm, gdh, zqdm, zqsl, mrje, mcje, ljyk, cccb, bdrq from securities.tZQGL where abs(MRJE-MCJE+LJYK-CCCB)>1 and BDRQ=to_char(sysdate, 'yyyymmdd')

--6.清算后资金冻结解冻数据检查
type=2
sql=select * from account.tZJDJXMMX where CXBZ!=2 and DJLB in(1, 2) and
FSRQ = to_char(sysdate, 'yyyymmdd') and YXRQ=to_char(sysdate, 'yyyymmdd') and
ROUND(TO_NUMBER(to_date(FSSJ,'hh24:mi:ss')-to_date('16:00:00','hh24:mi:ss'))*24*60*60)<0

--7.清算后资金可用数据检查
type=2
sql=select A.KHH, A.ZJZH, A.ZHYE, A.DJJE, B.DJJE, A.BDRQ from account.tZJZH A,
  (select zjzh,
    nvl( CASE WHEN greatest(max(DJJE), max(PREDJ))>=0 THEN greatest(max(DJJE),max(PREDJ))
    ELSE decode(sign(min(PRERQ)-to_char(sysdate, 'yyyymmdd')), 1, 0, greatest(max(DJJE),max(PREDJ))) END, 0) DJJE
  from
    (select ZJZH,DJJE,PRERQ,
      decode(sign(YXRQ-to_char(sysdate, 'yyyymmdd')), 1,PREDJ,DJJE) PREDJ from
      (select ZJZH, YXRQ, DJJE,
        first_value(YXRQ) over (partition by zjzh order by YXRQ rows between 1 preceding and 1 following) PRERQ,
      first_value(DJJE) over (partition by zjzh order by YXRQ rows between 1 preceding and 1 following) PREDJ,
      last_value(YXRQ) over (partition by zjzh order by YXRQ rows between 1 preceding and 1 following) LASTRQ
        from ( select ZJZH,
          SUM(sum(FSJE)) over(partition by zjzh order by yxrq range between unbounded preceding and current row) DJJE,
        YXRQ from account.tZJDJXMMX where  CXBZ<2 GROUP BY zjzh,yxrq ) )
          WHERE YXRQ>=to_char(sysdate, 'yyyymmdd') or YXRQ=LASTRQ )
    group by zjzh)
  B where A.ZJZH=B.ZJZH and A.DJJE<>B.DJJE and A.BDRQ=to_char(sysdate, 'yyyymmdd')
  union
  select KHH, ZJZH, ZHYE, DJJE, 0, BDRQ from account.tZJZH A where A.DJJE>0 and
    A.ZJZH not in (select zjzh from account.tZJDJXMMX where CXBZ<2) and A.BDRQ=to_char(sysdate, 'yyyymmdd')

--8.白天相同类型错误回报过多异常提醒（可能隐含系统有问题）
type=1
sql=select n1, n2, bl as 比例,
  (case
    when (sign(to_char(sysdate, 'hh24miss')-'091500')<0 or 
        (sign(to_char(sysdate, 'hh24miss')-'113000')>0 and
         sign(to_char(sysdate, 'hh24miss')-'130000')<0) or
        sign(to_char(sysdate, 'hh24miss')-'150000')>0)
  then '非交易时间有已申报数据'
  when (sign(to_char(sysdate, 'hh24miss')-'091500')>=0 and
        sign(to_char(sysdate, 'hh24miss')-'092500')<0)
    then '集合竞价时间有成交数据'
  when ((sign(to_char(sysdate, 'hh24miss')-'092500')>=0 and
         sign(to_char(sysdate, 'hh24miss')-'113000')<=0) or
      (sign(to_char(sysdate, 'hh24miss')-'130000')>=0 and
         sign(to_char(sysdate, 'hh24miss')-'150000')<=0))
    then '交易时间待申报数据过多或没有申报数据'
  else '未知提示' end) as 说明
  from 
  (select a.n n1, b.n n2, decode(b.n, 0, 1, a.n/b.n) bl from
    (select count(*) n from securities.tdrwt where 
    (
      (sign(to_char(sysdate, 'hh24miss')-'091500')<0 and
       (ROUND(TO_NUMBER(to_date(wtsj,'hh24:mi:ss')-to_date('09:15:00','hh24:mi:ss'))*24*60*60)<0 or
     wtrq<>to_char(sysdate, 'yyyymmdd'))and
     sbjg>1 and sbjg<8)
        or
      (sign(to_char(sysdate, 'hh24miss')-'091500')>=0 and
       sign(to_char(sysdate, 'hh24miss')-'092500')<0 and
     sbjg in(5,6,7))
      or
      (sign(to_char(sysdate, 'hh24miss')-'092500')>=0 and
       sign(to_char(sysdate, 'hh24miss')-'113000')<=0 and sbjg<2)
      or
      (sign(to_char(sysdate, 'hh24miss')-'113000')>0 and
       sign(to_char(sysdate, 'hh24miss')-'130000')<0 and
         ROUND(TO_NUMBER(to_date(wtsj,'hh24:mi:ss')-to_date('11:30:00','hh24:mi:ss'))*24*60*60)>0 and
         ROUND(TO_NUMBER(to_date(wtsj,'hh24:mi:ss')-to_date('13:00:00','hh24:mi:ss'))*24*60*60)<0 and
     sbjg in(5,6,7))
      or
      (sign(to_char(sysdate, 'hh24miss')-'130000')>=0 and
       sign(to_char(sysdate, 'hh24miss')-'150000')<=0 and sbjg<2)
      or
      (sign(to_char(sysdate, 'hh24miss')-'150000')>0 and
     sign(to_char(sysdate, 'hh24miss')-'153000')<=0 and
       ROUND(TO_NUMBER(to_date(wtsj,'hh24:mi:ss')-to_date('15:00:00','hh24:mi:ss'))*24*60*60)>0 and
     ROUND(TO_NUMBER(to_date(wtsj,'hh24:mi:ss')-to_date('15:30:00','hh24:mi:ss'))*24*60*60)<=0 and
     (wtlb<55 or wtlb>60) and sbjg>1 and sbjg<8)
    or
    (sign(to_char(sysdate, 'hh24miss')-'153000')>0 and
     ROUND(TO_NUMBER(to_date(wtsj,'hh24:mi:ss')-to_date('15:30:00','hh24:mi:ss'))*24*60*60)>0 and
     wtrq=to_char(sysdate, 'yyyymmdd') and sbjg>1 and sbjg<8)
    ) and zqdm<>'799999' and jys not in('HK', 'SK')) a,
    (select count(*) n from securities.tdrwt where zqdm<>'799999' and jys not in('HK', 'SK')) b)
where
  (sign(to_char(sysdate, 'hh24miss')-'091500')<0 and n1<>0)
  or
  (sign(to_char(sysdate, 'hh24miss') -'091500')>=0 and
   sign(to_char(sysdate, 'hh24miss')-'092500')<0 and n1<>0)
  or
  (sign(to_char(sysdate, 'hh24miss')-'092500')>=0 and
   sign(to_char(sysdate, 'hh24miss')-'113000')<=0 and bl>0.05)
  or
  (sign(to_char(sysdate, 'hh24miss')-'113000')>0 and
   sign(to_char(sysdate, 'hh24miss')-'130000')<0 and n1<>0)
  or
  (sign(to_char(sysdate, 'hh24miss')-'130000')>=0 and
   sign(to_char(sysdate, 'hh24miss')-'150000')<=0 and bl>0.05)
  or
  (sign(to_char(sysdate, 'hh24miss')-'150000')>0 and n1<>0)

--9.交易所回报信息检查，对一些可能是发送给交易所的数据有问题的交易所回报进行提醒（可能隐含系统有问题）
type=1
sql=select jys, jgsm, count(jgsm) n from securities.tDRWT where SBJG=3
group by jgsm, jys having count(jgsm)>100

--10.清算后股份冻结数据检查
type=2
sql=select * from securities.tZQDJXMMX  where  GFDJLB=1 and CZBZ!=2 and
FSRQ=to_char(sysdate, 'yyyymmdd') and ZDCXRQ=to_char(sysdate, 'yyyymmdd') and 
ROUND(TO_NUMBER(to_date(FSSJ,'hh24:mi:ss')-to_date('16:00:00','hh24:mi:ss'))*24*60*60)<0

--11.投票数量超过一定阀值进行提示
type=1
sql=select khh, khxm, jys, gdh, zqdm, sum(wtsl) from securities.tDRWT
where wtlb=23 group by zqdm, gdh, khh, khxm, jys having sum(wtsl)>10

--12.当日股票未出现买卖，相关买入卖出金额和数量出现变化
type=1
sql=select A.khh, A.khxm, A.gdh, A.zqdm, A.zqsl, A.mrje, A.mcje, A.ljyk, A.cccb, A.bdrq from securities.tZQGL A where (A.MRWTSL!=0 or A.MCWTSL!=0)
and A.BDRQ=to_char(sysdate, 'yyyymmdd') and A.ZQLB not in('A4', 'C4', 'Z0') and (A.ZQDM not in(select
B.ZQDM from securities.tDRWT B where A.KHH=B.KHH and A.JYS=B.JYS and A.GDH=B.GDH))
