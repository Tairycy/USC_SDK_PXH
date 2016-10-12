package cn.yunzhisheng.casr;

class EncodeContent{
        //private native void print(String para);
        public static void main(String[] args){
	    while(true){
                //String name=new String();
                //System.out.println(System.getProperty("java.library.path"));
                EncodeContent encoder=new EncodeContent();
		//byte[] key=new byte[10];
		String key, targetData;
		key="132jo2034224333";
		targetData="sbpb5lrl6aonjrpw4a56if6xcc6sd4mpes6rneqn\n<NAME>\n何金来\n任禾\n李佚杰\n</NAME>\n<APP>\n音乐\n照相机\n美图秀秀\n</APP>\n<SONG>\n红尘客栈\n青花瓷\n发如雪\n</SONG>\n";
		byte[] encodeData=new byte[3*targetData.length()];
		byte[] decodeData=new byte[3*targetData.length()];
		byte[] decodeKey=new byte[60];
		byte[] tmp;
		tmp=key.getBytes();
		encoder.EncodeTotalContent(key.getBytes(), targetData.getBytes(), encodeData);

		byte[] tmpIntEn=new byte[4];
		tmpIntEn[0]=encodeData[0];
                tmpIntEn[1]=encodeData[1];
                tmpIntEn[2]=encodeData[2];
                tmpIntEn[3]=encodeData[3];
                int encodedLen=BytesToInt(tmpIntEn);	
		//System.out.println("output encode data len is "+encodedLen);
		//String tmp;
		//tmp=encodeData.toString();
		//System.out.println("$$$$$$$$$$$$$$$$$$$$$$$$");
		//System.out.println(new String(encodeData));

		//System.out.println("$$$$$$$$$$$$$$$$$$$$$$$$");
		encoder.DecodeTotalContent(decodeKey, encodeData, decodeData);
		//System.out.println("************************");
		//String slen=new String(decodeData,0,4);
		//int ilen=java.lang.Integer.parseInt(slen);
		//int ilen=Integer.valueOf(slen);
		byte[] tmpInt=new byte[4];
		tmpInt[0]=decodeData[0];
		tmpInt[1]=decodeData[1];
		tmpInt[2]=decodeData[2];
		tmpInt[3]=decodeData[3];
		int ilen=BytesToInt(tmpInt);
		/*for (int i = 0; i < 4; i++) {
			ilen|=decodeData[i];
			ilen=ilen<<8;
       		}*/
		String sdecodeKey=new String(decodeKey, 1, decodeKey[0]);
		//System.out.println("decode key is "+sdecodeKey+"*****");
		String sLen=""+ilen;
		System.out.println("decode data Len is "+sLen);
		//System.out.println(new String(decodeData,4,ilen));

		int status=encoder.CheckKeyValid("sbpb5lrl6aonjrpw4a56if6xcc6sd4mpes6rneqn".getBytes());
		if(status==0)
			System.out.println("key check OK!");
		else
			System.out.println("key check fail!");
	    }
        }

	private native int EncodeTotalContent(byte[] key, byte[] targetData, byte[] encodedData);
	private native int DecodeTotalContent(byte[] key, byte[] encodedData, byte[]decodedData);
	private native int CheckKeyValid(byte[] key);
	//private native int DecodeContent(byte[] key, byte[] encodedData, byte[] decodedData);
	public static int BytesToInt(byte[] intByte)
        {
                int fromByte  = 0;

                for (int i = 0; i < 4; i++)
                {
                        int n = (intByte[i] < 0 ? (int)intByte[i] + 256 : (int)intByte[i]) << (8 * i);
                        fromByte += n;
                }
                return fromByte;
        }

        static {
               System.loadLibrary("EncodeContent");
        }
}
