import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Iterator;

public class FileOperation 
{
	public int[] readAttribute(String fn)
	{
		int input_count = 0;
		int output_count = 0;
		int result[] = {0,0};
		File f = new File(fn);
		String temp;
		try {
			BufferedReader reader = new BufferedReader(new FileReader(f));
			while((temp = reader.readLine()) != null)
			{ 
				if(temp.equals(""))
					continue;
				if(temp.substring(0, 2).equals("in"))
				{
					input_count++;
				}
				else if(temp.substring(0, 3).equals("out"))
				{
					output_count++;
				}
			}
			reader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		result[0] = input_count;
		result[1] = output_count;
		
		return result;
	}
	
	public Hashtable<String, String> readDataset(String fn)
	{
		File f = new File(fn);
		String temp;
		BufferedReader reader = null;
		String data[];
		String str1;
		String str2;

		Hashtable<String, String> ht = new Hashtable<String, String>();
		StringBuffer bs = new StringBuffer();
		try 
		{
			reader = new BufferedReader(new FileReader(f));
		
			while((temp = reader.readLine()) != null)
			{
				data = temp.split("  ");
				str1 = data[0];
				str2 = data[1];
				ht.put(str1.trim(), str2.trim());
			}
			reader.close();
		} 
		catch (FileNotFoundException e1) 
		{
			e1.printStackTrace();
		}
		catch (IOException e) 
		{
			e.printStackTrace();
		}
		
		return ht;
	}
	
	public String[][] readTennisAttribute(String fn)
	{
		int input_count = 0;
		int output_count = 2;
		int result[] = {0,0};
		File f = new File(fn);
		
		String temp;
		//4th is the output: yes/no
		String attr_value[][] = new String[5][]; // every attribute has several values
		
		int counter = 0;
		
		try {
			BufferedReader reader = new BufferedReader(new FileReader(f));

			while((temp = reader.readLine()) != null)
			{ 
				if(temp.equals(""))
					break;
				attr_value[counter++] = temp.split(" ");
			
			}
			reader.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		//result[0] = input_count;
		//result[1] = output_count;
		
		return attr_value;
	}

	public Hashtable<String, String> readTennisData(String fn1,String fn2)
	{
		String attr_values[][] = readTennisAttribute(fn1);
		File f = new File(fn2);
		Hashtable<String,String> ht = new Hashtable<String,String>();
		String temp;
		String values[];
		StringBuffer sb = null;
		int counter = 0;
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(f));

			while((temp = reader.readLine()) != null)
			{ 
				sb = new StringBuffer();
				values = temp.split(" ");
				for(int i = 0; i < values.length - 1; i++)
				{
					for(int j = 1; j <= attr_values[i].length - 1; j++)
					{
						if(attr_values[i][j].equals(values[i]))
						{
							sb.append(transfer(attr_values[i].length - 1,j - 1));

						}
					}
				}
				if(values[values.length - 1].equals("No"))
					ht.put(sb.toString(), "0 ");
				else if(values[values.length - 1].equals("Yes"))
					ht.put(sb.toString(), "1 ");
				else
					ht.put(sb.toString(), "0 ");
				sb = null;
			}
		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return ht;	
	}
	
	public Hashtable<String, String> readIrisData(String fn2)
	{
		File f = new File(fn2);
		Hashtable<String,String> ht = new Hashtable<String,String>();
		String temp;
		String values[];
		StringBuffer sb = null;
		int counter = 0;
		
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(f));


			while((temp = reader.readLine()) != null)
			{ 
				sb = new StringBuffer();
				values = temp.split(" ");
				for(int i = 0; i < values.length - 1; i++)
				{
					sb.append(values[i]);
					sb.append(" ");
				}


				if(values[values.length - 1].equals("Iris-setosa"))
					ht.put(sb.toString(), "0 0 1");
				else if(values[values.length - 1].equals("Iris-versicolor"))
					ht.put(sb.toString(), "1 0 0");
				else if(values[values.length - 1].equals("Iris-virginica"))
					ht.put(sb.toString(), "0 1 0");

				sb = null;
			}
		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return ht;	
	}
	
	public StringBuffer transfer(int total, int index)
	{
		StringBuffer sb = new StringBuffer();
		int r = (index+1) * 10;
		String temp = String.valueOf(r);
		sb.append(temp);
		sb.append(" ");
		return sb;
	}
	
	public void corruptIrisTrainingSet(Hashtable<String, String> ht, float persent)
	{
		int size = ht.size();
		int num = (int)(size * persent); // indicate how many labels should be corrupted

		Iterator<String> it = ht.keySet().iterator();
		String val_array[];
		int count = 0;
		StringBuffer result = null;
		while(it.hasNext())
		{
			result = new StringBuffer();
			if(count >= num)
			{
				break;
			}
			String key = it.next();
			String value = ht.get(key);
			if(value.equals("0 0 1"))
				value = "1 0 0";
			else if(value.equals("1 0 0"))
				value = "0 0 1";
			else if(value.equals("0 1 0"))
				value = "0 0 1";
			/*
			val_array = value.split(" ");
			
			int i = Math.abs(Integer.valueOf(val_array[0]) - 1);  // turn 1 to 0, or turn 0 to 1
			val_array[0] = String.valueOf(i);
			for(int a = 0; a < val_array.length; a++)
			{
				result.append(val_array[a]);
				if(a == val_array.length - 1)
					break;
				result.append(" ");
			}*/
			ht.put(key, value);
			count++;
		}
	
	}
	public Hashtable<String, String> readDiscrete(String fn)
	{
		return null; 
	}
}
