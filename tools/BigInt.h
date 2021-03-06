namespace BigMath
{
  enum BigMathERROR { BigMathMEM = 1 , BigMathOVERFLOW , BigMathUNDERFLOW, BigMathINVALIDINTEGER, BigMathDIVIDEBYZERO,BigMathDomain};

  const char *BigIntErrDes[] = { "Allocation Failed", "Overflow","Underflow", "Invalid Integer", "Divide by Zero" ,"Domain Error", NULL};
  const char BigIntPROGRAMNAME[] = { "BigInteger" };
  const int BigIntMajorVersion = 6;
  const int BigIntMinorVersion = 7;
  const int BigIntRevision = 28;
  const char LastUpdated[] = {"July 30, 2004"};
  const char AuthorName[] = {"S. M. Mahbub Murshed Suman"};
  const char *AuthorEmail[] = {"udvranto@yahoo.com","suman@bttb.net.bd", NULL};

  void Dump(const char *,enum BigMathERROR);
  string& DumpString (char const*,enum BigMathERROR);
  
  typedef unsigned int SizeT;
  typedef unsigned int DATATYPE;
  const DATATYPE BASE = 10000;
  const DATATYPE INVALIDDATA = 65535U;
  const SizeT LOG10BASE = 4;
  class BigInteger
  {
  private:
    DATATYPE *TheNumber;
    SizeT Start;
    SizeT End;
    bool isNegative;
    BigInteger(SizeT,DATATYPE,bool);
    void datacopy(BigInteger const&,SizeT);
    SizeT datalen(DATATYPE const*) const;
    void deallocateBigInteger();
    void TrimZeros();
    void Set(DATATYPE);
	DATATYPE& operator[](unsigned int) const;
  public:
    BigInteger();
    BigInteger(long);
    BigInteger(char const*);
    BigInteger(BigInteger const&);
    ~BigInteger();
    int UnsignedCompareTo(BigInteger const&)const;
    int CompareTo(BigInteger const&)const;
    SizeT Digits() const;
    bool isValidNumber() const;
    bool isZero()const;
    BigInteger& Add(BigInteger const&) const;
    BigInteger& Subtract(BigInteger const&) const;
    BigInteger& Multiply(BigInteger const&) const;
    BigInteger& Multiply(DATATYPE const&) const;
    BigInteger& DivideAndRemainder(BigInteger const&,BigInteger&,bool) const;
    BigInteger& DivideAndRemainder(DATATYPE const&,DATATYPE&,bool) const;
    friend BigInteger& operator+(BigInteger const&, BigInteger const&);
    friend BigInteger& operator-(BigInteger const&, BigInteger const&);
    friend BigInteger& operator*(BigInteger const&, BigInteger const&);
    friend BigInteger& operator*(BigInteger const&, DATATYPE const&);
    friend BigInteger& operator*(DATATYPE const&, BigInteger const&);
    friend BigInteger& DivideAndRemainder(BigInteger const&, BigInteger const&,BigInteger&,bool);
    friend BigInteger& DivideAndRemainder(BigInteger const&, DATATYPE const&,DATATYPE&,bool);
    friend BigInteger& operator/(BigInteger const&, BigInteger const&);
    friend BigInteger& operator/(BigInteger const&, DATATYPE const&);
    friend BigInteger& operator/(DATATYPE const&, BigInteger const&);
    friend BigInteger& operator%(BigInteger const&, BigInteger const&);
    friend BigInteger& operator%(BigInteger const&, DATATYPE const&);
    friend BigInteger& operator%(DATATYPE const&, BigInteger const&);
    BigInteger& operator=(BigInteger const&);
    friend ostream& operator<<(ostream& ,  BigInteger const&);
    friend istream& operator>>(istream& , BigInteger& );


    BigInteger& operator++();
    BigInteger& operator++(int);

    BigInteger& operator--();
    BigInteger& operator--(int);

    BigInteger& operator-();


    BigInteger& operator<<(SizeT);
    BigInteger& operator>>(SizeT);
    void abs();
    friend BigInteger& abs(BigInteger&);

    string& toString() const;
    int toInt();
    long toLong();

    BigInteger& Power(long )const;
    BigInteger& OldSquareRoot() const;
	BigInteger& SquareRoot() const;
  };

  BigInteger::BigInteger(SizeT bytes,DATATYPE fill, bool toInit = true)
  {
    TheNumber = new DATATYPE[bytes];
    isNegative = false;
    Start = 0;
    End = bytes - 1;

    if(toInit) Set(fill);
  }

  BigInteger::BigInteger()
  {
    TheNumber = new DATATYPE[1];
    TheNumber[0] = 0;
    Start = 0;
    End = 0;
    isNegative = false;
  }

  BigInteger::BigInteger(long n)
  {
    if(n<0)
    {
      isNegative = true;
      n *= -1;
    }
    else isNegative = false;

    SizeT i = (SizeT)(floor(log10((double)n)/LOG10BASE) + 1);

    Start = 0;
    End = i-1;
    TheNumber = new DATATYPE [i];
    Set(0);

    while(n)
    {
      TheNumber[--i] = n % BASE;
      n /= BASE;
    }
    TrimZeros();
  }

  BigInteger::BigInteger(char const* n)
  {
    if(n[0]=='-') { isNegative = true; n++; }
    else if(n[0]=='+') { isNegative = false; n++; }
    else isNegative = false;

    while(*n=='0') n++;

    int l = strlen(n);
    if(l==0)
    {
*this = *new BigInteger();
      return;
    }
    Start = 0;
    End = (SizeT)(l/LOG10BASE + l%LOG10BASE - 1);
    TheNumber = new DATATYPE [Digits()];
    Set(0);

    int cur = l - 1;
    for(SizeT i = End; i>=Start;i--)
    {
      if(cur<0) break;
        DATATYPE r = 0;
      DATATYPE TEN = 1;
      for(l=0;l<4;l++)
      {
        if(cur<0) break;
        r = r + TEN*(n[cur]-'0');
        TEN *= 10;
        cur--;
      }
      TheNumber[i] = r;
    }
    TrimZeros();
    if(isZero()) isNegative = false;
  }

  BigInteger::BigInteger(BigInteger const& Temp)
  {
    Start = 0;
    End = Temp.Digits() - 1;
    TheNumber = new DATATYPE [Temp.Digits()];

    datacopy(Temp,Temp.Digits());
    isNegative = Temp.isNegative;
  }

  void BigInteger::deallocateBigInteger()
  {
    if(TheNumber!=0)
    {
      delete [] TheNumber;
      TheNumber = 0;
    }
  }

  BigInteger::~BigInteger()
  {
    deallocateBigInteger();
    Start = 0;
    End = 0;
  }

  void BigInteger::Set(DATATYPE fill)
  {
    for(SizeT i=Start;i<=End;i++)
      TheNumber[i] = fill;
  }

  void BigInteger::datacopy(BigInteger const& a,SizeT size)
  {
    for(SizeT i=0;i<size;i++)
      TheNumber[Start+i] = a.TheNumber[a.Start+i];
  }

  SizeT BigInteger::datalen(DATATYPE const* a) const
  {
    SizeT l = 0;
    while(a[l]!=INVALIDDATA) l++;
    return l;
  }

  SizeT BigInteger::Digits() const
  { return End-Start+1; }

  bool BigInteger::isZero() const
  { return End==Start && TheNumber[Start]==0; }

  bool BigInteger::isValidNumber() const
  {
    for(SizeT i=Start; i<End ; i++)
      if(TheNumber[i]>=BASE) return false;

    return true;
  }

  void BigInteger::TrimZeros()
  {
    while(TheNumber[Start]==0 && Start<End)
      Start++;
  }

  int BigInteger::UnsignedCompareTo(BigInteger const& with)const
  {
    if(isZero() && with.isZero()) return 0;
    if(!isZero() && with.isZero()) return 1;
    if(isZero() && !with.isZero()) return -1;

    long temp = Digits() - with.Digits();
    if(temp!=0) return temp<0?-1:1;


    temp = 0;
    int cmp = 0;
    for(SizeT i=0;i<Digits();i++)
    {
      temp = TheNumber[i+Start] - with.TheNumber[i+with.Start];
      if(temp!=0)
      {
        cmp = temp<0?-1:1;
        break;
      }
    }

    return cmp;
  }

  int BigInteger::CompareTo(BigInteger const& with)const
  {
    int cmp = UnsignedCompareTo(with);

    if(isNegative==false && with.isNegative==true) return 1;
    if(isNegative==true && with.isNegative==false) return -1;
    int both_neg = 1;
    if(isNegative==true && with.isNegative==true) both_neg = -1;
    return cmp*both_neg;
  }

  BigInteger& BigInteger::Add(BigInteger const& Small) const
  {
    BigInteger const& Big = *this;
    BigInteger &Result= *new BigInteger(Big.Digits()+1,0);

    long Carry=0,Plus;
    long i=Big.Digits() - 1,
      j=Small.Digits() -1;

    for(; i>=0 ;i--,j--){
      Plus = Big.TheNumber[i+Big.Start] + Carry;
      if(j>=0) Plus += Small.TheNumber[j+Small.Start] ;

      Result.TheNumber[i+1] = Plus%BASE;
      Carry = Plus/BASE;
    }
    i++;

    if(Carry) Result.TheNumber[i--] = Carry;

    Result.TrimZeros();

    return Result;
  }

  BigInteger& BigInteger::Subtract(BigInteger const& Small)const
  {
    BigInteger const& Big = *this;
    BigInteger& Result = *new BigInteger(Big.Digits()+1,0);

    long Carry=0,Minus;

    long i = Big.Digits() - 1,
      j= Small.Digits() - 1;

    for( ; i>=0 ;i--,j--)
    {
      Minus = Big.TheNumber[i+Big.Start] - Carry;
      if(j>=0) Minus -= Small.TheNumber[j+Small.Start];

      if(Minus < 0)
      {
        Result.TheNumber[i+1] = Minus + BASE;
        Carry = 1;
      }
      else
      {
        Result.TheNumber[i+1]  = Minus;
        Carry = 0;
      }
    }
    Result.TrimZeros();
    return Result;
  }

  BigInteger& operator+(BigInteger const& N1, BigInteger const& N2)
  {
    if(N1.isNegative && !N2.isNegative)
    {
      BigInteger& Res = *new BigInteger(N1);
      Res.isNegative = false;
      Res = N2-Res;
      return Res;
    }
    if(!N1.isNegative && N2.isNegative)
    {
      BigInteger& Res = *new BigInteger(N2);
      Res.isNegative = false;
      Res = N1-Res;
      return Res;
    }

    BigInteger& ret = *new BigInteger();

    if(N1.UnsignedCompareTo(N2)<0)
      ret = N2.Add(N1);
    else
      ret = N1.Add(N2);
    if(N1.isNegative==true && N2.isNegative==true)
      ret.isNegative = true;
    return ret;
  }

  BigInteger& operator-(BigInteger const& N1, BigInteger const& N2)
  {
    if(N1.isNegative && !N2.isNegative)
    {
      BigInteger& res = *new BigInteger(N1);
      res.isNegative = false;
      res = res+N2;
      res.isNegative = true;
      return res;
    }
    if(!N1.isNegative && N2.isNegative)
    {
      BigInteger& res = *new BigInteger(N2);
      res.isNegative = false;
      res = res+N1;
      return res;
    }


    BigInteger& ret = *new BigInteger();
    int cmp = N1.UnsignedCompareTo(N2);
    if(cmp==0)
    {
      ret = *new BigInteger();
    }
    if(cmp<0)
    {
      ret = N2.Subtract(N1);
      ret.isNegative = true;
    }
    else
    {
      ret = N1.Subtract(N2);
      ret.isNegative = false;
    }
    return ret;
  }

  BigInteger& BigInteger::Multiply(BigInteger const& Small) const
  {
    BigInteger const& Big = *this;
    BigInteger& Result = *new BigInteger(Big.Digits()+Small.Digits(),0);

    long Carry,Multiply;

    SizeT i;
    SizeT j;

    for(i = 0 ; i< Small.Digits() ; i++)
    {
      Carry = 0;
      for(j = 0 ; j< Big.Digits() ; j++)
      {
        Multiply = ( (long)Small.TheNumber[Small.End-i] * (long)Big.TheNumber[Big.End-j] )
          + Carry + Result.TheNumber[Result.End-i-j];
        Result.TheNumber[Result.End-i-j] = Multiply%BASE;
        Carry = Multiply/BASE ;
      }
      Result.TheNumber[Result.End-i-j] = Carry;
    }

    Result.TrimZeros();

    return Result;
  }

  BigInteger& BigInteger::Multiply(DATATYPE const& with) const
  {
    BigInteger& Result = *new BigInteger(Digits()+1,0);

    long Carry,Multiply;

    SizeT i;

    Carry = 0;
    for(i = 0 ; i< Digits() ; i++)
    {
      Multiply = Carry + (long)TheNumber[End-i] * (long)with;
      Carry = Multiply / BASE;
      Result.TheNumber[Result.End-i] = Multiply % BASE;
    }
    Result.TheNumber[Result.End-i] = Carry;
    Result.TrimZeros();

    return Result;
  }

  BigInteger& operator*(BigInteger const& N1, BigInteger const& N2)
  {
    if(N1.isZero() || N2.isZero()) return *new BigInteger();
    BigInteger& ret = *new BigInteger();
    if(N1.UnsignedCompareTo(N2)<0)
      ret = N2.Multiply(N1);
    else
      ret = N1.Multiply(N2);
    if(N1.isNegative!=N2.isNegative)
      ret. isNegative = true;
    return ret;
  }

  BigInteger& operator*(BigInteger const& N1, DATATYPE const& N2)
  {
    if(N1.isZero() || N2==0) return *new BigInteger();
    BigInteger& ret = N1.Multiply(N2);
    return ret;
  }

  BigInteger& operator*(DATATYPE const& N1, BigInteger const& N2)
  {
    if(N2.isZero() || N1==0) return *new BigInteger();
    BigInteger& ret = N2.Multiply(N1);
    return ret;
  }

  BigInteger& BigInteger::DivideAndRemainder(DATATYPE const& V,DATATYPE& _R,bool skipRemainder=false) const
  {
    BigInteger& W = *new BigInteger(Digits(),0,false);
    DATATYPE R = 0;
    SizeT j;
    for(j=0;j<=End;j++)
    {
      W.TheNumber[j] = (R*BASE+TheNumber[Start+j])/V;
      R = (R*BASE+TheNumber[Start+j])%V;
    }

    W.TrimZeros();
    if(skipRemainder==false)
      _R = R;

    return W;
  }

  BigInteger& BigInteger::DivideAndRemainder(BigInteger const& _V,BigInteger& R,bool skipRemainder=false) const
  {
    SizeT m = this->Digits()-_V.Digits();
    SizeT n = _V.Digits();
    BigInteger& Q = *new BigInteger(m+1,0,false);

    DATATYPE d, qhat, rhat;
    long temp,x,y;
    SizeT i;
    int j;

    d = (BASE-1)/_V.TheNumber[_V.Start];

    BigInteger& U = this->Multiply(d);
    BigInteger& V = _V.Multiply(d);

    for(j = m; j>=0; j--)
    {
      temp = (long)U.TheNumber[U.End-j-n]*(long)BASE + (long)U.TheNumber[U.End-j-n+1];
      x = temp / (long)V.TheNumber[V.Start];
      y = temp % (long)V.TheNumber[V.Start];
      if(x>(long)BASE) x /= BASE;
      if(y>(long)BASE) y %= BASE;
      qhat = (DATATYPE) x;
      rhat = (DATATYPE) y;

      bool badRhat = false;
      do
      {
        x = (long)qhat * (long)V.TheNumber[V.Start+1];
        y = (long)BASE*(long)rhat + (long)U.TheNumber[U.End-j-n+1];

        if(qhat==BASE || x > y)
        {
          qhat --;
          rhat += V.TheNumber[V.Start];
          if(rhat<BASE) badRhat = true;
          else badRhat = false;
        }
        else break;
      }while(badRhat);

      x = 0;
      temp = 0;
      for(i=0;i<=n;i++)
      {
        if(V.End>=i) temp = (long)qhat*(long)V.TheNumber[V.End-i] + temp;
        y = (long)U.TheNumber[U.End-j-i] - temp%BASE - x;
        temp /= BASE;
        if(y < 0)
        {
          U.TheNumber[U.End-j-i] = (DATATYPE)(y+BASE);
          x = 1;
        }
        else
        {
          U.TheNumber[U.End-j-i]  = (DATATYPE)y;
          x = 0;
        }
      }

      Q.TheNumber[Q.End-j] = qhat;

      if(x)
      {
        Q.TheNumber[Q.End-j] --;
        x = 0;
        for(i=0;i<=n;i++)
        {
          y = (long)U.TheNumber[U.End-j-i] + x;
          if(V.End>=i) y += (long)V.TheNumber[V.End-i];
          U.TheNumber[U.End-j-i] = (DATATYPE)(y % BASE);
          x = y / BASE;
        }
        U.TheNumber[U.End-j-i] = (DATATYPE)x;
      }
    }

    U.TrimZeros();
    DATATYPE _t;
    if(skipRemainder==false)
      R = U.DivideAndRemainder(d,_t,true);
    Q.TrimZeros();

    return Q;
  }

  BigInteger& DivideAndRemainder(BigInteger const& U, BigInteger const& V,BigInteger& R,bool skipRemainder=false)
  {
//      throw logic_error (DumpString ("DivideAndRemainder (BigInteger/BigInteger)",BigMathDIVIDEBYZERO));

    if(U.isZero())
    {
      R = *new BigInteger();
      return *new BigInteger();
    }

    int cmp = U.UnsignedCompareTo(V);
    if(cmp==0)
    {
      R = *new BigInteger();
      BigInteger& W = *new BigInteger(1l);
      if(U.isNegative!=V.isNegative) W.isNegative = true;
      return W;
    }
    else if(cmp<0)
    {
      R = *new BigInteger(U);
      if(U.isNegative!=V.isNegative) R.isNegative = true;
      return *new BigInteger();
    }
    BigInteger& ret = U.DivideAndRemainder(V,R,skipRemainder);
    if(U.isNegative!=V.isNegative) ret.isNegative = true;
    return ret;
  }

  BigInteger& DivideAndRemainder(BigInteger const& U, DATATYPE const& V,DATATYPE& R,bool skipRemainder=false)
  {
//      throw logic_error ( DumpString ("DivideAndRemainder (BigInteger/DATATYPE)",BigMathDIVIDEBYZERO) );

    if(U.isZero())
    {
      R = 0;
      return *new BigInteger();
    }

    int cmp = 1;
    if(U.Digits()==1)
    {
        if(U.TheNumber[U.Start]<V) cmp = -1;
        else if(U.TheNumber[U.Start]>V) cmp = 1;
        else cmp = 0;
    }
    if(cmp==0)
    {
      R = 0;
      return *new BigInteger(1l);
    }
    else if(cmp<0)
    {
      R = U.TheNumber[U.Start];
      return *new BigInteger();
    }
    BigInteger& ret = U.DivideAndRemainder(V,R,skipRemainder);
    return ret;
  }

  BigInteger& operator/(BigInteger const& N1, BigInteger const& N2)
  {
    BigInteger& R = *new BigInteger();
    return DivideAndRemainder(N1,N2,R,true);
  }

  BigInteger& operator/(BigInteger const& U, DATATYPE const& V)
  {
    DATATYPE R;
    return DivideAndRemainder(U,V,R,true);
  }

  BigInteger& operator/(DATATYPE const& U, BigInteger const& V)
  {
    if(V.Digits()==1 && U>V.TheNumber[V.Start])
    {
      return *new BigInteger(U/V.TheNumber[V.Start]);
    }
    return *new BigInteger();
  }

  BigInteger& operator%(BigInteger const& N1,BigInteger const& N2)
  {
    BigInteger& R = *new BigInteger();
    DivideAndRemainder(N1,N2,R);
    return R;
  }

  BigInteger& operator%(BigInteger const& U, DATATYPE const& V)
  {
    DATATYPE R;
    DivideAndRemainder(U,V,R);
    return *new BigInteger(R);
  }

  BigInteger& operator%(DATATYPE const& U, BigInteger const& V)
  {
    if(V.Digits()==1 && U>V.TheNumber[V.Start])
    {
      return *new BigInteger(U%V.TheNumber[V.Start]);
    }
    return *new BigInteger();
  }

  BigInteger& BigInteger::operator=(BigInteger const&arg)
  {
    Start = 0;
    End = arg.Digits() - 1;
    delete [] TheNumber;
    TheNumber = new DATATYPE [arg.Digits()];

    datacopy(arg,arg.Digits());
    isNegative = arg.isNegative;
    return *this;
  }

  ostream& operator<<(ostream& stream,  BigInteger const& out)
  {
    if(out.isNegative==true && out.isZero()==false) stream << '-';
    stream << out.TheNumber[out.Start];
    for(SizeT i=out.Start+1;i<=out.End;i++)
    {
      stream.width(4);
      stream.fill('0');
      stream << out.TheNumber[i];
    }

    return stream;
  }

  istream& operator>>(istream& stream, BigInteger& in)
  {
    SizeT SIZE = 500;
    char *data = new char[SIZE];
    SizeT i = 0;
    int input;
    bool isNegative = false;

    if(stream.eof())
    	return stream;
    stream >> ws;

    if(stream.eof())
    	return stream;
    input = stream.get();

    if(input=='-')
    	isNegative = true;
    else if(input=='+')
    	isNegative = false;
    else
    	stream.putback(input);

    while(true)
    {
      input = stream.get();
      if(stream.eof())
      	break;

      if(isdigit(input))
        data[i++] = input;
      else
      {
        stream.putback(input);
        break;
      }
      if(i>=SIZE)
      {
        SIZE += SIZE;
        char *p = new char [SIZE];
        strcpy(p,data);
        delete [] data;
        data = p;
      }
    }
    data[i] = 0;
    in = *new BigInteger(data);
    if(in.isZero()==false)
        in.isNegative = isNegative;
    return stream;
  }

  ostream& BigIntegerVersion(ostream& out)
  {
    out << BigIntPROGRAMNAME << " (Version "<< BigIntMajorVersion
      << "." << BigIntMinorVersion << "." << BigIntRevision << ")";
    return out;
  }

  ostream& BigIntegerAuthor(ostream& out)
  {
    out << "Author: " << AuthorName << endl
    	<< "mailto: ";
	int i = 0;
	while(AuthorEmail[i]!=NULL)
	{
		if(i!=0) out << ", ";
		out << AuthorEmail[i++];
	}
    return out;
  }

  ostream& BigIntegerLastUpdate(ostream& out)
  {
    out << "Last Updated: " << LastUpdated;
    return out;
  }

  ostream& BigIntegerAbout(ostream& out)
  {
    out << BigIntegerVersion << endl
    	<< BigIntegerLastUpdate << endl
    	<< BigIntegerAuthor << endl;
    return out;
  }

  string& BigIntegerVersionString()
  {
    char out[100];
    ostrstream str(out,sizeof(out));
    str << BigIntegerVersion;
    return *new string(out);
  }

  string& BigInteger::toString()const
  {
    const int DIGITS = Digits()*4;
    char *R = new char[DIGITS+2];
    ostrstream ostr(R,DIGITS);
    ostr << *this;
    return *new string(R);
  }

  int BigInteger::toInt()
  {
    return (int)toLong();
  }

  long BigInteger::toLong()
  {
    long r = TheNumber[End];
    if(Digits()>1) r += BASE * TheNumber[End-1] ;
    if(Digits()>2) r += BASE*BASE*(TheNumber[End-2]%100);
    return r;
  }

  BigInteger& BigInteger::operator++()
  {
    BigInteger& ONE = *new BigInteger(1l);
*this = *this+ONE;
    return *this;
  }

  BigInteger& BigInteger::operator++(int notused)
  {
    BigInteger one(1l);
    BigInteger& Ret = *new BigInteger(*this);
*this = *this+one;
    return Ret;
  }

  BigInteger& BigInteger::operator--()
  {
    BigInteger one(1l);
*this = *this-one;
    return *this;
  }

  BigInteger& BigInteger::operator--(int notused)
  {
    BigInteger one(1l);
    BigInteger& Ret = *new BigInteger(*this);
*this = *this-one;
    return Ret;
  }

  BigInteger& BigInteger::operator-()
  {
    this->isNegative = this->isNegative==true?false:true;
    return *this;
  }

  DATATYPE& BigInteger::operator[](unsigned int pos) const
  {

	  return TheNumber[pos];
  }

  BigInteger& BigInteger::operator<<(SizeT b)
  {
    SizeT i = (SizeT)(b / LOG10BASE);
    b %= LOG10BASE;
    while(b--) TheNumber[Digits()-1] *= 10;

    if(i>0)
    {
      DATATYPE *temp = new DATATYPE[Digits()+i];
      for(b=0;b<Digits();b++)
        temp[b] = TheNumber[b];
      for(b=0;b<i; b++)
        temp[Digits()+b] = 0;
      delete [] TheNumber;
      TheNumber = temp;
      End += i-1;
    }
    return *this;
  }

  BigInteger& BigInteger::operator>>(SizeT b)
  {
    SizeT i = (SizeT)(b / LOG10BASE);
    b %= LOG10BASE;
    End -= i;
    while(b--)
    {
      DATATYPE f = 0,l = 0;
      for(i=Start;i<=End;i++)
      {
        l = TheNumber[i]%10;
        TheNumber[i] = f*BASE + TheNumber[i]/10;
        f = l;
      }
      if(TheNumber[Start]==0)
        Start--;
    }
    return *this;
  }

  void BigInteger::abs()
    { isNegative = false; }

  BigInteger& abs(BigInteger& arg)
  {
      BigInteger& ret = *new BigInteger(arg);
      ret.isNegative = false;
      return ret;
  }

  BigInteger& BigInteger::Power(long y) const
  {
    long P;
    BigInteger& pow = *new BigInteger(1l);
    BigInteger& z = *new BigInteger();
    BigInteger& ZERO = *new BigInteger();
    BigInteger& ONE = *new BigInteger(1l);
    P = y;

    if(y==0) return pow;
    if(isZero()) return ZERO;
    if(UnsignedCompareTo(ONE)==0) return ONE;

    z = *this;
    while(P>0)
    {
      while(P%2==0)
      {
        P /= 2;
        z = z*z;
      }
      P--;
      pow = pow*z;
    }
    return pow;
  }

  void Dump(char const* s,enum BigMathERROR e)
  {
    cerr << BigIntegerVersion << endl;
    cerr << "Exception: " << BigIntErrDes[e-1] << endl;
    cerr << "In function: " << s  << endl;
    cerr << "Error Code: " << e << endl;
    exit(e);
  }

  string& DumpString (char const* s,enum BigMathERROR e)
  {
    char *R = new char[256];
    ostrstream ostr(R,255);
    ostr << BigIntegerVersion << endl;
    ostr << "Exception: " << BigIntErrDes[e-1] << endl;
    ostr << "In function: " << s  << endl;
    ostr << "Error Code: " << e << endl;
    return *new string(R);
  }

  BigInteger& BigInteger::OldSquareRoot() const
  {
    BigInteger const& n = *this;
    BigInteger& _2 = *new BigInteger(2l);
    BigInteger& zero = *new BigInteger();
    if(n.isZero())
      return zero;

//      throw domain_error ( DumpString ("SquareRoot",BigMathDomain) );

    long Dig;
    if(n.Digits()%2==0)
      Dig = n.Digits()/2 - 1;
    else
      Dig = n.Digits()/2 ;

    BigInteger& sq = *new BigInteger(1l);
    if(Dig==-1)
      return sq;

    BigInteger sqr,toIncrease,temp;
    sq = sq << Dig;
    toIncrease = sq;

    while(true)
    {
      sqr = sq*sq;
      if(sqr.CompareTo(n) == 0) break;
      if(toIncrease.CompareTo(zero)==0) break;
      if(sqr.CompareTo(n) > 0)
      {
        toIncrease = toIncrease/_2;
        sq = temp;
      }

      temp = sq;
      sq = sq + toIncrease;
    }
    return sq;
  }

  BigInteger& BigInteger::SquareRoot() const
  {
    if(isZero())
      return *new BigInteger();


	BigInteger& result = *new BigInteger(Digits(),0,false);
	SizeT i = 1;

	result[result.Start] = sqrt((double)TheNumber[Start]);
	BigInteger& remainder = *new BigInteger(
		TheNumber[Start] - result[result.Start] * result[result.Start] );
	BigInteger& temp = *new BigInteger(result);
	temp.Multiply(2);

	delete &temp;
	delete &remainder;

    return result;
  }

  bool operator==(BigInteger const& a, BigInteger const& b)
  { return a.CompareTo(b)==0; }

  bool operator!=(BigInteger const& a, BigInteger const& b)
  { return a.CompareTo(b)!=0; }

  bool operator>=(BigInteger const& a, BigInteger const& b)
  { return a.CompareTo(b)>=0; }

  bool operator<=(BigInteger const& a, BigInteger const& b)
  { return a.CompareTo(b)<=0; }

  bool operator>(BigInteger const& a, BigInteger const& b)
  { return a.CompareTo(b)>0; }

  bool operator<(BigInteger const& a, BigInteger const& b)
  { return a.CompareTo(b)<0; }

  typedef BigInteger bint;
}
