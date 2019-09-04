# Create a mac sniffer using arduino ESP8266 / Wemos d1 mini / Wemos d1 mini pro.

## Promiscuous fake ssid
## Post data collected in a json to a http server

Example to receive it with PHP - Laravel 

Route api:
```php  
  Route::apiResource('nodes', 'NodeController');
```

Node Model
```php
  class Node extends Model
  {
      protected $fillable = [
          'read_at',
          'cluster_id',
          'rssi',
          'mac',
      ];


      public function cluster(){
          return $this->belongsTo(Cluster::class); // maybe create the cluster info? 
      }

  }
```

Cluster Model
```php
  class Cluster extends Model
  {
      protected $fillable = [
          'name'
      ];


      public function cluster(){
          return $this->hasOne(Node::class);
      }
  }
```

Node Controller
```php
  /**
   * @return \Illuminate\Http\Resources\Json\AnonymousResourceCollection
   */
  public function index()
  {
      return NodeResource::collection(Node::with('cluster')->paginate(25));
  }
  

  /**
   * @param Request $request
   * @return NodeResource
   */
  public function store(Request $request)
  {
      if(!empty($request->probes)){
          foreach ($request->probes as $item){
              Node::create([
                  'cluster_id' => $item['cluster_id'],
                  'rssi' => $item['rssi'],
                  'mac' => $item['mac'],
                  'read_at' => Carbon::now()->format('Y-m-d H:i:s'),
              ]);
          }
          return response()->json('Bulk data posted.', 200);
      }
      return response()->json('empty', 200);
  }
```

    

